#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

SemaphoreHandle_t sem;
unsigned char dato = 0;

void task1(){
	while(1){
		dato++;
		xSemaphoreGive(sem);
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

void task2(){
	while(1){
		xSemaphoreTake(sem, portMAX_DELAY);
		printf("Dato = %d\n\r", dato);
	}
}

void app_main()
{
	sem = xSemaphoreCreateBinary();

	xTaskCreate(task1, "task1", 2000, NULL, 4, NULL);
	xTaskCreate(task2, "task2", 2000, NULL, 3, NULL);
}
