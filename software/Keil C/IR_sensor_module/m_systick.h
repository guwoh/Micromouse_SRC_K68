#pragma once
#include "stm32f4xx.h"
void systick_init_ms(uint32_t sysclk_hz);
void delay_ms(uint32_t ms);
