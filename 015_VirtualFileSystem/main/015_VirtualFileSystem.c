#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "string.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "nvs_flash.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

#define WIFI_SSID "linux"
#define WIFI_PASS "s5F1iqZM"

static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

#define true 1
#define false 0

xSemaphoreHandle wifi_on;

// Wifi event handler
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	printf("Eventos de Wifi %d\r\n", event->event_id);
    switch(event->event_id){

       case SYSTEM_EVENT_STA_START:
           esp_wifi_connect();
           break;

	   case SYSTEM_EVENT_STA_GOT_IP:
          xSemaphoreGive(wifi_on);
          break;

	   default:
          break;
    }

	return ESP_OK;
}


// Main task
void main_task()
{

	DIR *dr = NULL;
	struct dirent *entrada_directory;
	FILE *archivo = NULL;

	char buff[50] = {0};

	while(1){
		xSemaphoreTake(wifi_on, portMAX_DELAY);
		printf("Esp Conectado OK\r\n");
		printf("Creando SD Virtual\r\n");

		esp_vfs_fat_mount_config_t mount_config = {
			.max_files = 4,
			.format_if_mount_failed = false
		};

		if(esp_vfs_fat_spiflash_mount("/vsd", "storage", &mount_config, &s_wl_handle) == ESP_OK)
		{
			printf("virtual file system [OK]\r\n");
			dr = opendir("/vsd");
			if(dr == NULL){
				printf("no se encontró el directorio\r\n");
			}
			else{
				while((entrada_directory = readdir(dr)) != NULL){
					printf("%s\r\n", entrada_directory->d_name);
				}
				closedir(dr);
			}



			//Leer Archivo
			/*archivo = fopen("/vsd/hola.txt", "r");
			if(archivo != NULL){
				printf("archivo encontrado [OK]\r\n");
				fgets(buff, sizeof(buff), archivo);
				printf("%s\r\n", buff);*/

			//Escribir Archivo
				/*fprintf(archivo, "Texto escrito en VSD en ESP32\r\n");
				fclose(archivo);*/
		}
		else
			printf("Error\r\n");

		while(1){
			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
	}
}


// Main application
void app_main()
{
	//Deshabilitamos log eventos wifi
	esp_log_level_set("wifi", ESP_LOG_NONE);

	//Inicialización NVS (Non Volatile Storage)
	ESP_ERROR_CHECK(nvs_flash_init());

	//Inicialización de Stack TCP
	tcpip_adapter_init();

	//Inicialización de Driver WiFi
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	//Inicialización WiFi
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); //Selección de modo Station

	//Configurar SSID y PASS
	wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

	//Creacion Semaphoro Binario
	wifi_on = xSemaphoreCreateBinary();

	//Tarea principal
    xTaskCreate(main_task, "main_task", 20000, NULL, 5, NULL);
}
