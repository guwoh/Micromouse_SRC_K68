#include "m_gpio.h"

static inline uint32_t port_index(GPIO_TypeDef *port) {
  return ((uint32_t)port - (uint32_t)GPIOA) / ((uint32_t)GPIOB - (uint32_t)GPIOA);
}

void gpio_enable_clock(GPIO_TypeDef *port) {
  RCC->AHB1ENR |= (1U << port_index(port)); (void)RCC->AHB1ENR;
}

void gpio_mode(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode) {
  port->MODER = (port->MODER & ~(3U << (pin*2))) | ((uint32_t)mode << (pin*2));
}

void gpio_otype_pp(GPIO_TypeDef *port, uint8_t pin) {
  port->OTYPER &= ~(1U << pin);
}

void gpio_ospeed_high(GPIO_TypeDef *port, uint8_t pin) {
  port->OSPEEDR |= (3U << (pin*2));
}

void gpio_nopull(GPIO_TypeDef *port, uint8_t pin) {
  port->PUPDR &= ~(3U << (pin*2));
}

void gpio_write(GPIO_TypeDef *port, uint8_t pin, int level) {
  if (level) port->BSRR = (1U << pin);
  else       port->BSRR = (1U << (pin+16));
}

void gpio_toggle(GPIO_TypeDef *port, uint8_t pin) {
  port->ODR ^= (1U << pin);
}

void gpio_set_af(GPIO_TypeDef *port, uint8_t pin, uint8_t af) {
  if (pin < 8) {
    port->AFR[0] = (port->AFR[0] & ~(0xFU << (pin*4))) | ((uint32_t)af << (pin*4));
  } else {
    uint8_t p = pin - 8;
    port->AFR[1] = (port->AFR[1] & ~(0xFU << (p*4))) | ((uint32_t)af << (p*4));
  }
}


// (Gi? s? b?n dã có hàm gpio_mode, gpio_nopull...)
// Ho?c vi?t m?t hàm chuyên d?ng:
void gpio_config_analog(GPIO_TypeDef* port, uint8_t pin) {
    // 1. Ð?t ch? d? là Analog (MODER[1:0] = 11)
    port->MODER |= (3UL << (pin * 2)); // 3UL = 11 (binary)
    
    // 2. Không c?n kéo lên/kéo xu?ng (PUPDR[1:0] = 00)
    port->PUPDR &= ~(3UL << (pin * 2));
}