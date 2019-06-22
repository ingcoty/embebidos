#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

xSemaphoreHandle wifi_on;
xSemaphoreHandle mqtt;

//#define WIFI_SSID "Wi-Fi Unimagdalena"
#define WIFI_SSID "linux"
#define WIFI_PASS "s5F1iqZM"

char buff[50] = {0};

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            esp_mqtt_client_subscribe(event->client, "embebidos2", 0);
            break;

        case MQTT_EVENT_SUBSCRIBED:
        	xSemaphoreGive(mqtt);
            break;

        case MQTT_EVENT_DATA:
        	memcpy(buff, event->data, event->data_len);
        	printf("%s\r\n", buff);

            break;

        default:
			break;
    }
    return ESP_OK;
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
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

void main_task(void)
{
    xSemaphoreTake(wifi_on, portMAX_DELAY);
    printf("Conectando a Broker\r\n");

	esp_mqtt_client_config_t mqtt_cfg = {
        .host = "10.42.0.1",
		//.uri = "mqtt://test.mosquitto.org",
        .event_handle = mqtt_event_handler,
		.lwt_topic = "testamento",
		.lwt_msg = "Electricaribe HP",
		.keepalive = 10,
		.disable_clean_session = 1
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

    xSemaphoreTake(mqtt, portMAX_DELAY);
    while(1){
    	esp_mqtt_client_publish(client, "embebidos", "mqtt_esp", 0, 2, 0);
    	vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}

void app_main()
{
	nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_on = xSemaphoreCreateBinary();
    mqtt = xSemaphoreCreateBinary();

    xTaskCreate(main_task, "mqtt_task", 20000, NULL, 2, NULL);
}

