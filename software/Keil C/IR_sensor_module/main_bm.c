#include "stm32f4xx.h"
#include "m_clock.h"
#include "m_systick.h"
#include "m_gpio.h"
#include "m_pins.h" 

#include "m_dma.h"   // <-- Ðã có
#include "m_adc.h"   // <-- Ðã có

// Ð?nh nghia b? d?m
#define ADC_BUFFER_SIZE 50
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];


int main(void) {
    // 1) Clock
    clock_init_84mhz_from_hsi(); // Gi? s? hàm này cung b?t clock cho GPIO, DMA, ADC

    // 2) SysTick
    systick_init_ms(SystemCoreClock);

    // 3) C?u hình chân PA0 (ADC1_IN0) là Analog
    gpio_enable_clock(GPIOA); // Hàm này ch?c b?n dã có trong m_gpio.c
    gpio_config_analog(GPIOA, 0); // Hàm m?i

    // 4) C?u hình DMA (Ph?i ch?y tru?c ADC)
    // C?n b?t clock cho DMA2 TRU?C khi g?i hàm này
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    dma_adc1_config((uint32_t)adc_buffer, ADC_BUFFER_SIZE); 

    // 5) C?u hình ADC
    // C?n b?t clock cho ADC1 TRU?C khi g?i hàm này
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    adc1_config();

    // 6) B?t d?u ADC
    adc1_start();
    
    while (1) {
        // M?i th? ch?y ng?m!
        // CPU r?nh r?i.
        // M?ng adc_buffer dang du?c t? d?ng n?p d?y.
        
        // B?n có th? thêm code tính trung bình ? dây
        // delay_ms(100); // Ð?i 100ms
        // uint32_t avg = 0;
        // for(int i=0; i < ADC_BUFFER_SIZE; i++) {
        //     avg += adc_buffer[i];
        // }
        // avg = avg / ADC_BUFFER_SIZE;
        // (Làm gì dó v?i bi?n 'avg')
    }
}