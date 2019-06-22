#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>

xSemaphoreHandle inter;

void delay_ms(int ms){
	vTaskDelay(ms/portTICK_PERIOD_MS);
}

void isr_handler(int param){
	xSemaphoreGiveFromISR(inter, pdTRUE);
}

void task1(){
	while(1){
		xSemaphoreTake(inter, portMAX_DELAY);
		gpio_set_level(22, 1);
		delay_ms(500);
		gpio_set_level(22, 0);
		delay_ms(500);
	}
}

void task2(){
	while(1){
		gpio_set_level(2, 1);
		delay_ms(50);
		gpio_set_level(2, 0);
		delay_ms(50);
	}
}

void app_main()
{
	inter = xSemaphoreCreateBinary();

	gpio_config_t gpio_conf;
	gpio_conf.intr_type = GPIO_INTR_POSEDGE;
	gpio_conf.mode = GPIO_MODE_INPUT;
	gpio_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
	gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_conf.pin_bit_mask = (1UL << 23);
	gpio_config(&gpio_conf);

	gpio_conf.intr_type = GPIO_INTR_DISABLE;
	gpio_conf.mode = GPIO_MODE_OUTPUT;
	gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_conf.pin_bit_mask = (1UL << 22 | 1UL << 2);
	gpio_config(&gpio_conf);

	gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
	gpio_isr_handler_add(23, isr_handler, (int)23);

	xTaskCreate(task1, "task1", 2000, NULL, 3, NULL);
	xTaskCreate(task2, "task2", 2000, NULL, 5, NULL);
}
