/*
 * Mode.h
 *
 *  Created on: Oct 2, 2025
 *      Author: kinhz
 */

#ifndef INC_MODE_H_
#define INC_MODE_H_

#include "Encoder.h"
#include "Motor.h"
#include "delay.h"
#include "function_c.h"

extern uint16_t SPEED;
extern uint8_t BTN_STATE1;
extern uint8_t BTN_STATE2;

void mode();
void mouse_move();
void blink_led(uint8_t numb_blk);

#endif /* INC_MODE_H_ */
