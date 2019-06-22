#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <driver/gpio.h>

void Task1(){
	while(1)
	{
		gpio_set_level(22, 1);
		vTaskDelay(200/portTICK_PERIOD_MS);
		gpio_set_level(22, 0);
		vTaskDelay(200/portTICK_PERIOD_MS);
	}
}

void Task2(){
	while(1)
	{
		gpio_set_level(23, 1);
		vTaskDelay(500/portTICK_PERIOD_MS);
		gpio_set_level(23, 0);
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

void app_main(){

	gpio_set_direction(23, GPIO_MODE_OUTPUT);
	gpio_set_direction(22, GPIO_MODE_OUTPUT);

	xTaskCreate(&Task1, "Tarea1",  2000, NULL, 1, NULL);
	xTaskCreate(&Task2, "Tarea2",  2000, NULL, 1, NULL);
}
