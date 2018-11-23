#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "nvs_flash.h"

#define WIFI_SSID "smartlist"
#define WIFI_PASS "smartlist"


xSemaphoreHandle wifi_on;

// Wifi event handler
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	printf("Eventos de Wifi %d\r\n", event->event_id);
    switch(event->event_id) {

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
	const char reg_ok[] = "HTTP/1.1 200 OK";
	int sock = 0;
	int res = 0;
	char buff_rec[5000] = {0};
	const char request[] = "GET /Service1.svc/SetRegistro/1,2946031899,12-04-2018,12_00,ebKVhLxl HTTP/1.1\r\n"
						   "Host: grupotic.unimagdalena.edu.co:8091\r\n"
						   "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
			               "Accept-Encoding: gzip, deflate\r\n"
					       "Accept-Language: es-ES,es;q=0.9,en;q=0.8,it;q=0.7\r\n"
					       "\r\n";

	printf("Esperando Conexion\r\n");
			xSemaphoreTake(wifi_on, portMAX_DELAY);
			printf("ESP32 Conectado  [OK] \r\n");

	while(1){

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port =	htons(8091);
		addr.sin_addr.s_addr = inet_addr("10.32.0.30");
		sock = socket(AF_INET, SOCK_STREAM, 0);

		res = connect(sock, (struct sockaddr*)&addr, sizeof(addr));

		if(res >= 0){
			printf("conexion Existosa\r\n");
			send(sock, request, strlen(request), 0);

			if(recv(sock, buff_rec, sizeof(buff_rec), 0)>0){
				if(!strncmp(buff_rec, reg_ok, strlen(reg_ok))){
					printf("Registro [OK]\r\n");
				}
				else{
					printf("Error en el Registro\r\n");
				}
			}
			memset(buff_rec, 0, sizeof(buff_rec));
			close(sock);
		}
		else printf("Error de conexion \r\n");

		while(1){
			vTaskDelay(1000);
		}
	}
}


// Main application
void app_main()
{
	// disable the default wifi logging
	esp_log_level_set("wifi", ESP_LOG_NONE);

	// initialize NVS
	ESP_ERROR_CHECK(nvs_flash_init());

	// create the event group to handle wifi events
	wifi_on = xSemaphoreCreateBinary();

	// initialize the tcp stack
	tcpip_adapter_init();

	// initialize the wifi event handler
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	// initialize the wifi stack in STAtion mode with config in RAM
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// configure the wifi connection and start the interface
	wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
	printf("Conectando a red %s\n", WIFI_SSID);

	// start the main task
    xTaskCreate(main_task, "main_task", 20000, NULL, 5, NULL);
}
