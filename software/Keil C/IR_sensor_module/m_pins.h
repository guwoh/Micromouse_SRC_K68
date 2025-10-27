#pragma once
#include "stm32f4xx.h"

typedef enum {
  m_GPIO_MODE_INPUT = 0,
  m_GPIO_MODE_OUTPUT = 1,
  m_GPIO_MODE_AF = 2,
  m_GPIO_MODE_ANALOG = 3
} gpio_mode_t;

void gpio_enable_clock(GPIO_TypeDef *port);
void gpio_mode(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode);
void gpio_otype_pp(GPIO_TypeDef *port, uint8_t pin);
void gpio_ospeed_high(GPIO_TypeDef *port, uint8_t pin);
void gpio_nopull(GPIO_TypeDef *port, uint8_t pin);
void gpio_write(GPIO_TypeDef *port, uint8_t pin, int level);
void gpio_toggle(GPIO_TypeDef *port, uint8_t pin);
void gpio_set_af(GPIO_TypeDef *port, uint8_t pin, uint8_t af);
