// File: m_adc.c

#include "m_adc.h"

/**
 * @brief C?u hình ADC1 d? quét Kênh 0 (PA0) liên t?c v?i DMA.
 */
void adc1_config(void) {
    // 1. B?t clock cho ADC1 (dã làm ? main.c ho?c RCC_Config)
    // RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    
    // 2. C?u hình ch? d? chung (Common settings)
    ADC->CCR = 0; // Ð?t v? 0, không c?n prescaler ph?c t?p
    
    // 3. C?u hình CR1 (Control Register 1)
    ADC1->CR1 = 0;
    ADC1->CR1 &= ~ADC_CR1_RES; // Ð?t là 12-bit (giá tr? 00)
    
    // 4. C?u hình CR2 (Control Register 2)
    ADC1->CR2 = 0;
    
    // CONT = 1: Ch? d? chuy?n d?i liên t?c
    ADC1->CR2 |= ADC_CR2_CONT;
    
    // DMA = 1: B?t ch? d? DMA
    ADC1->CR2 |= ADC_CR2_DMA;
    
    // DDS = 1: T?o yêu c?u DMA liên t?c (m?i khi EOC = 1)
    ADC1->CR2 |= ADC_CR2_DDS;
    
    // ALIGN = 0: Can l? ph?i (Right alignment)
    ADC1->CR2 &= ~ADC_CR2_ALIGN;
    
    // 5. C?u hình th?i gian l?y m?u (Sample Time)
    // Cho kênh 0 (PA0), d?t th?i gian l?y m?u là 480 chu k? (cho ?n d?nh)
    ADC1->SMPR2 |= ADC_SMPR2_SMP0; // 111 = 480 cycles
    
    // 6. C?u hình trình t? quét (Sequence)
    // Ch? quét 1 kênh, là kênh 0
    ADC1->SQR1 = 0; // L[3:0] = 0000 (t?c là 1 chuy?n d?i)
    ADC1->SQR3 = 0; // SQ1 = Kênh 0
    
    // 7. B?t ADC
    ADC1->CR2 |= ADC_CR2_ADON;
}

/**
 * @brief B?t d?u quá trình chuy?n d?i ADC1.
 */
void adc1_start(void) {
    // Ghi bit SWSTART (Start a conversion) d? kh?i d?ng
    ADC1->CR2 |= ADC_CR2_SWSTART;
}