#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

xQueueHandle cola;

void Delay_ms(int ms){
	vTaskDelay(ms/portTICK_PERIOD_MS);
}

void task1(){
	char dato1 = 1;
	char dato2 = 2;
	char dato3 = 3;
	char dato4 = 4;
	while(1){
		if(xQueueSend(cola, &dato1, 0))printf("Dato 1 OK\r\n"); else printf("ERR 1\r\n");
		if(xQueueSend(cola, &dato2, 0))printf("Dato 2 OK\r\n"); else printf("ERR 2\r\n");
		if(xQueueSend(cola, &dato3, 0))printf("Dato 3 OK\r\n"); else printf("ERR 3\r\n");
		if(xQueueSend(cola, &dato4, 0))printf("Dato 4 OK\r\n"); else printf("ERR 4\r\n");
		Delay_ms(1000);
	}
}

void task2(){
	char dato_rec = 0;
	while(1){
		Delay_ms(1000);
		xQueueReceive(cola, &dato_rec , portMAX_DELAY);
		printf("dato recibido %d\r\n", dato_rec);
	}
}

void app_main()
{
	cola = xQueueCreate(3, sizeof(char));
	xTaskCreate(task1, "task1", 2000, NULL, 5, NULL);
	xTaskCreate(task2, "task2", 2000, NULL, 3, NULL);
}
