/*
 * interface.h
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

#define R_LED_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
#define L_LED_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
#define R_LED_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
#define L_LED_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

extern uint8_t BTN1_Flag;
extern uint8_t BTN2_Flag;
uint8_t BTN1();
uint8_t BTN2();
void start();
void blink_led_wait();

#endif /* INC_INTERFACE_H_ */
