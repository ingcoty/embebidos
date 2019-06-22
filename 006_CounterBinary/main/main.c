#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

xSemaphoreHandle sem;

void delay_ms(int ms){
	vTaskDelay(ms/portTICK_PERIOD_MS);
}

void task1(){
	while(1){
		if(xSemaphoreTake(sem, portMAX_DELAY))printf("la Tarea 1 tomó el semaforo 1 vez\r\n");
		if(xSemaphoreTake(sem, portMAX_DELAY))printf("la Tarea 1 tomó el semaforo 2 vez\r\n");
		if(xSemaphoreTake(sem, portMAX_DELAY))printf("la Tarea 1 tomó el semaforo 3 vez\r\n");

	}
}

void task2(){
	while(1){
		vTaskDelay(200);
		printf("Entregando el sem\r\n");
		xSemaphoreGive(sem);
	}
}

void app_main()
{
	sem = xSemaphoreCreateCounting(3, 0);
	xTaskCreate(task1, "task1", 2000, NULL, 4, NULL);
	xTaskCreate(task2, "task2", 2000, NULL, 3, NULL);
}
