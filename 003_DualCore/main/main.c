#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

void delay_ms(int ms){
	vTaskDelay(ms/portTICK_PERIOD_MS);
}

void task1(){
	while(1){
		printf("Esta es la tarea 1 corriend en el core %d\r\n", xPortGetCoreID());
		delay_ms(500);
	}
}

void task2(){
	while(1){
		printf("Esta es la tarea 2 corriend en el core %d\r\n", xPortGetCoreID());
		delay_ms(500);
	}
}

void app_main()
{
	xTaskCreatePinnedToCore(task1, "task1", 2000, NULL, 4, NULL, 1);
	xTaskCreatePinnedToCore(task2, "task2", 2000, NULL, 3, NULL, 0);
}
