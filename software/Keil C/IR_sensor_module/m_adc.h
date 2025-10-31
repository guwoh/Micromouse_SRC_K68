// File: m_adc.h

#ifndef __M_ADC_H
#define __M_ADC_H

#include "stm32f4xx.h"

// --- Khai báo hàm ---

/**
 * @brief C?u hình ADC1 d? quét Kênh 0 (PA0) liên t?c v?i DMA.
 */
void adc1_config(void);

/**
 * @brief B?t d?u quá trình chuy?n d?i ADC1.
 */
void adc1_start(void);

#endif // __M_ADC_H