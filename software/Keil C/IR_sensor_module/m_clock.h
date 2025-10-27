#pragma once
#include "stm32f4xx.h"
void clock_init_84mhz_from_hsi(void);
extern uint32_t SystemCoreClock; // dùng l?i bi?n c?a CMSIS
