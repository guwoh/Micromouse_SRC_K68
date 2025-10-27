#include "stm32f4xx.h"
#include "m_clock.h"
#include "m_systick.h"
#include "m_gpio.h"
#include "m_pins.h"

int main(void) {
  // 1) Clock 84MHz t? HSI
  clock_init_84mhz_from_hsi();

  // 2) SysTick 1ms
  systick_init_ms(SystemCoreClock);
/*
  // 3) LED PC13 output (nhi?u board F4 mini: active-low)
  gpio_enable_clock(LED_PORT);
  gpio_mode(LED_PORT, LED_PIN, m_GPIO_MODE_OUTPUT);
  gpio_otype_pp(LED_PORT, LED_PIN);
  gpio_ospeed_high(LED_PORT, LED_PIN);
  gpio_nopull(LED_PORT, LED_PIN);

  // 4) IR LED PC2 output (ví d?)
  gpio_enable_clock(IR_LED_PORT);
  gpio_mode(IR_LED_PORT, IR_LED_PIN, m_GPIO_MODE_OUTPUT);
  gpio_otype_pp(IR_LED_PORT, IR_LED_PIN);
  gpio_ospeed_high(IR_LED_PORT, IR_LED_PIN);
  gpio_nopull(IR_LED_PORT, IR_LED_PIN);
*/
  while (1) {
	/*
    // Toggle LED status
    gpio_toggle(LED_PORT, LED_PIN);
    // B?t t?t IR LED m?i l?n nháy
    static int s;
    gpio_write(IR_LED_PORT, IR_LED_PIN, (s ^= 1));
    delay_ms(200); */
  }
}
