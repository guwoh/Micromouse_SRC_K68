#include "stm32f4xx.h"
#include <stdint.h>

static void delay_ms(uint32_t ms) {
  for (uint32_t i = 0; i < ms * 4000; ++i) __NOP(); // gi? d?nh HSI 16 MHz
}

static void gpio_enable_clock(GPIO_TypeDef *port) {
  if (port == GPIOC) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
  (void)RCC->AHB1ENR;
}

static void gpio_init_output(GPIO_TypeDef *port, uint8_t pin) {
  gpio_enable_clock(port);
  port->MODER   = (port->MODER & ~(3U << (pin*2))) | (1U << (pin*2));
  port->OTYPER &= ~(1U << pin);
  port->OSPEEDR|=  (3U << (pin*2));
  port->PUPDR   &= ~(3U << (pin*2));
}

int main(void) {                     // ph?i dúng y h?t nhu này
  // SystemInit() dã du?c g?i t? startup tru?c khi vào main
  gpio_init_output(GPIOC, 13);
  while (1) {
    GPIOC->ODR ^= (1U << 13);
    delay_ms(200);
  }
}
