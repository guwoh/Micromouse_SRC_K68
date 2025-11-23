/*
 * Mode.c
 *
 *  Created on: Oct 2, 2025
 *      Author: kinhz
 */

#include "Mode.h"

uint16_t SPEED = 200;
uint8_t BTN_STATE1 = 0;
uint8_t BTN_STATE2 = 0;

void mode() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
	while(BTN_STATE1 < 1) {
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) {
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			wall_front_calibration();
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) {
			BTN_STATE1 = 1;
		}
	}
	blink_led(2);
}

void mouse_move() {
	switch (get(&mouse_state, mouse_state.index)) {
	case 0:
		Motor_stop();
		break;
	case 1:
		Move_onecell();
		break;
	case 2:
		delay_ms(400);
		Turn_right90_2();
		delay_ms(400);
		break;
	case 3:
		delay_ms(400);
		Turn_left90_2();
		delay_ms(400);
		break;
	default:
		break;
	}
}

void blink_led(uint8_t numb_blk) {
	for(uint8_t i = 0; i < numb_blk; i++) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
		delay_ms(500);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);
		delay_ms(500);
	}
}
