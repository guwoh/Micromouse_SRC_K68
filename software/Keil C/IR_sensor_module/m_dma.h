// File: m_dma.h

#ifndef __M_DMA_H
#define __M_DMA_H

#include "stm32f4xx.h"

// --- Khai báo hàm ---

/**
 * @brief C?u hình DMA2, Stream 0, Channel 0 d? nh?n d? li?u t? ADC1.
 * @param buffer_address Ð?a ch? c?a b? d?m trong RAM.
 * @param buffer_size Kích thu?c c?a b? d?m (s? lu?ng m?u).
 */
void dma_adc1_config(uint32_t buffer_address, uint32_t buffer_size);


#endif // __M_DMA_H