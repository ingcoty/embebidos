#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

void delay_ms(int ms){
	vTaskDelay(ms/portTICK_PERIOD_MS);
}

void task1(){
	while(1){
		gpio_set_level(22, 1);
		delay_ms(200);
		gpio_set_level(22, 0);
		delay_ms(200);
	}
}

void task2(){
	while(1){
	   gpio_set_level(23, 1);
	   delay_ms(500);
	   gpio_set_level(23, 0);
	   delay_ms(500);
	}
}

void task3(){
	while(1){
	   printf("level pin 22 = %d\r\n", gpio_get_level(22));
	   printf("level pin 23 = %d\r\n", gpio_get_level(23));
	   delay_ms(100);
	}
}

void app_main()
{
	gpio_config_t gpio_conf;
	gpio_conf.intr_type = GPIO_INTR_DISABLE;
	gpio_conf.mode = GPIO_MODE_INPUT_OUTPUT;
	gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_conf.pin_bit_mask = (1UL << 23 | 1UL << 22);

	gpio_config(&gpio_conf);


	xTaskCreate(task1, "task1", 2000, NULL, 3, NULL);
	xTaskCreate(task2, "task2", 2000, NULL, 3, NULL);
	xTaskCreate(task3, "task2", 2000, NULL, 2, NULL);
}
