#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "time.h"

void task1(){
	time_t tiempo;
	struct tm timestruct;

	while(1){
		time(&tiempo);
		localtime_r(&tiempo, &timestruct);
		printf("Time = %d segundos %d minutos \n", timestruct.tm_sec, timestruct.tm_min);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

/*
void task1(){
	TickType_t start_time = xTaskGetTickCount();
	int frec = xt_clock_freq();
	while(1){
		printf("Esta es la tarea 1 %d\n", start_time);
		printf("CPU a una frecuencia de %d\n", frec);
		vTaskDelay(100);
		start_time = xTaskGetTickCount();
	}
}
*/

void app_main()
{
	xTaskCreate(task1, "task1", 2000, NULL, 3, NULL);
}
