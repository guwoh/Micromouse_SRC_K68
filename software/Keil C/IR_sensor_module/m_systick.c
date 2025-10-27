#include "m_systick.h"

static volatile uint32_t s_ticks;

void SysTick_Handler(void) {
  s_ticks++;
}

void systick_init_ms(uint32_t sysclk_hz) {
  s_ticks = 0;
  SysTick->LOAD  = (sysclk_hz / 1000U) - 1U; // 1ms
  SysTick->VAL   = 0;
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | // AHB = SystemCoreClock
                   SysTick_CTRL_TICKINT_Msk   | // b?t IRQ
                   SysTick_CTRL_ENABLE_Msk;     // ch?y
  NVIC_SetPriority(SysTick_IRQn, 15);
}

void delay_ms(uint32_t ms) {
  uint32_t start = s_ticks;
  while ((uint32_t)(s_ticks - start) < ms) { __NOP(); }
}
