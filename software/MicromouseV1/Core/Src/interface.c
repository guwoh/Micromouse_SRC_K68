/*
 * interface.c
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#include "main.h"

uint8_t BTN1_Flag = 0;
uint8_t BTN2_Flag = 0;

uint8_t BTN1() {
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
}
uint8_t BTN2() {
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8);
}
void start(ADC_HandleTypeDef *hadc1) {
	while (LFSensor < LFThreshold1) {
		if (controlFlag) {
			controlFlag = 0;
			read_sensor(hadc1);
		}
	}
}

void blink_led_wait() {
	for(uint8_t i = 0; i < 10; i ++) {
		L_LED_ON;
		delay_ms(200);
		L_LED_OFF;
		delay_ms(200);
	}
}


