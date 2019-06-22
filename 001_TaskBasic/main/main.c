#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void delay_ms(int ms){
	vTaskDelay(ms/portTICK_PERIOD_MS);
}

void task1(){
	while(1){

	   printf("Esto es la tarea 1 nucleo %d\r\n", xPortGetCoreID());
	   delay_ms(500);
	}
}

void task2(){
	while(1){
	   printf("Esto es la tarea 2 nucleo %d\r\n", xPortGetCoreID());
	   delay_ms(500);
	}
}

void app_main()
{
	xTaskCreatePinnedToCore(task1, "task1", 2000, NULL, 4, NULL, 0);
	xTaskCreatePinnedToCore(task2, "task2", 2000, NULL, 3, NULL, 1);
}
