#include "m_clock.h"

void clock_init_84mhz_from_hsi(void) {
  // 1) B?t HSI (m?c d?nh dã ON), chu?n b? Flash
  FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_2WS;

  // 2) C?u hình PLL: HSI(16MHz)/M=16 -> 1MHz; 1MHz*N=336MHz; /P=4 -> SYSCLK=84MHz; /Q=7 -> 48MHz (USB)
  RCC->CR |= RCC_CR_HSION;
  while (!(RCC->CR & RCC_CR_HSIRDY));

  RCC->PLLCFGR =
      (16U << RCC_PLLCFGR_PLLM_Pos) |   // M = 16
      (336U << RCC_PLLCFGR_PLLN_Pos) |  // N = 336
      (0U << RCC_PLLCFGR_PLLP_Pos) |    // P = 2 -> t?m set, lát n?a d?i? (trên F4: 00->2, 01->4, 10->6, 11->8)
      RCC_PLLCFGR_PLLSRC_HSI |
      (7U << RCC_PLLCFGR_PLLQ_Pos);

  // d?i P = 4: bit PLLP = 01
  RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLP) | (1U << RCC_PLLCFGR_PLLP_Pos); // P=4

  // 3) B?t PLL
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY));

  // 4) Prescaler: AHB=1, APB1=2 (<=42MHz), APB2=1
  RCC->CFGR =
      RCC_CFGR_HPRE_DIV1 |
      RCC_CFGR_PPRE1_DIV2 |
      RCC_CFGR_PPRE2_DIV1;

  // 5) Ch?n SYSCLK = PLL
  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  // 6) C?p nh?t bi?n SystemCoreClock (d? các module khác dùng)
  SystemCoreClock = 84000000U;
}
