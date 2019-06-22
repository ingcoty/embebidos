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

#include "tcpip_adapter.h"
#include "esp_event.h"

#define WIFI_SSID "linux"
#define WIFI_PASS "s5F1iqZM"

static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

xSemaphoreHandle client_connected;

// Wifi event handler
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	printf("Eventos de Wifi %d\r\n", event->event_id);
    switch(event->event_id){

       case SYSTEM_EVENT_AP_START:
           printf("AP inicializado OK");
           break;

	   case SYSTEM_EVENT_AP_STACONNECTED:
          xSemaphoreGive(client_connected);
          break;

	   default:
          break;
    }

	return ESP_OK;
}


// Main task
void main_task()
{
	int sock, sock_client, c;
	struct sockaddr_in server, client;
	unsigned char buffer[50] = {0};

	while(1){
		xSemaphoreTake(client_connected, portMAX_DELAY);
		printf("Cliente Conectado\r\n");
		while(1){
			sock = socket(AF_INET, SOCK_STREAM, 0);

			server.sin_family = AF_INET;
			server.sin_addr.s_addr = INADDR_ANY;
			server.sin_port = htons(80);

			if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
				printf("Error de binding\r\n");
			}

			printf("Enlace Ok\r\n");

			listen(sock, 3);

			while(1){
				printf("Esperando Conexiones\r\n");
				c = sizeof(struct sockaddr_in);
				sock_client = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c);
				if (sock_client < 0)
				{
					printf("conexión rechazada");
					return;
				}

				printf("Cliente Conectado\r\n");
				while(1){
					memset(buffer, 0, sizeof(buffer));
					//Recibir mensajes
					recv(sock_client , buffer , sizeof(buffer) , 0);
					printf("%s", buffer);
				}
				close(sock_client);


			}
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

	tcpip_adapter_init();

	ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
	tcpip_adapter_ip_info_t info;
	memset(&info, 0, sizeof(info));
	IP4_ADDR(&info.ip, 192, 168, 10, 1);
	IP4_ADDR(&info.gw, 192, 168, 10, 1);
	IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));


	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

	wifi_auth_mode_t auth = WIFI_AUTH_OPEN;

	wifi_config_t ap_config = {
		.ap = {
			.ssid = "Wifi_esp32",
			.password = "embebidos",
			.ssid_len = 0,
			.channel = 10,
			.authmode = auth,
			.ssid_hidden = 0,
			.max_connection = 4,
			.beacon_interval = 100,
		},
	};

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	printf("Inicializando AP\r\n");

	client_connected = xSemaphoreCreateBinary();

	//Tarea principal
    xTaskCreate(main_task, "main_task", 20000, NULL, 5, NULL);
}
