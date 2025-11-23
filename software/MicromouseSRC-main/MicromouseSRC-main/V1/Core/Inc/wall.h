/*
 * wall.h
 *
 *  Created on: Feb 28, 2025
 *      Author: kinhz
 */

#ifndef INC_WALL_H_
#define INC_WALL_H_

#include "main.h"
#include "stm32f1xx_hal.h"
#include "Sensor.h"

uint8_t wall_left();
uint8_t wall_right();
uint8_t wall_front();

#endif /* INC_WALL_H_ */
