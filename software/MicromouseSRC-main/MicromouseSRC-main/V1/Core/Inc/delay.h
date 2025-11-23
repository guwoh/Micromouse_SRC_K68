/*
 * delay.h
 *
 *  Created on: Sep 17, 2025
 *      Author: kinhz
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stdbool.h" //for bool
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"
#include "Sensor.h"

void Systick_Configuration(void);//initialize systick
void delay_ms(uint32_t nTime);
void delay_us(uint32_t nTime);
uint32_t micros(void);
uint32_t millis(void);
void elapseMicros(uint32_t targetTime, uint32_t oldt);
void elapseMillis(uint32_t targetTime, uint32_t oldt);

extern volatile uint32_t Millis;
extern volatile uint32_t Micros;

#define useExternalOSC 1    //true=1/false=0
#define systemFrequency 72   //MHz

#endif /* INC_DELAY_H_ */
