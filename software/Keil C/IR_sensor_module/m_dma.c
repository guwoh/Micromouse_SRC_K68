// File: m_dma.c

#include "m_dma.h"

/**
 * @brief C?u hình DMA2, Stream 0, Channel 0 d? nh?n d? li?u t? ADC1.
 * @param buffer_address Ð?a ch? c?a b? d?m trong RAM.
 * @param buffer_size Kích thu?c c?a b? d?m (s? lu?ng m?u).
 */
void dma_adc1_config(uint32_t buffer_address, uint32_t buffer_size) {
    
    // 1. B?t clock cho DMA2 (dã làm ? main.c ho?c RCC_Config)
    // RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // 2. T?t Stream 0 di tru?c khi c?u hình (n?u nó dang ch?y)
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    // Ch? cho d?n khi bit EN th?c s? b? xóa
    while(DMA2_Stream0->CR & DMA_SxCR_EN);

    // 3. PAR (Peripheral Address): Ð?a ch? ngu?n (luôn là thanh ghi d? li?u c?a ADC)
    DMA2_Stream0->PAR = (uint32_t)&(ADC1->DR);
    
    // 4. M0AR (Memory 0 Address): Ð?a ch? dích (dùng tham s? truy?n vào)
    DMA2_Stream0->M0AR = buffer_address;
    
    // 5. NDTR (Number of Data): S? lu?ng d? li?u c?n chuy?n (dùng tham s? truy?n vào)
    DMA2_Stream0->NDTR = buffer_size;
    
    // 6. CR (Configuration Register): C?u hình lu?ng
    
    // Xóa thanh ghi CR v? 0
    DMA2_Stream0->CR = 0;
    
    // CHSEL[2:0] = 000: Ch?n Channel 0 (vì ADC1 n?m ? Channel 0)
    DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL; 
    
    // DIR[1:0] = 00: Hu?ng: Ngo?i vi -> B? nh? (Peripheral-to-memory)
    DMA2_Stream0->CR &= ~DMA_SxCR_DIR;
    
    // CIRC = 1: Ch? d? Vòng (Circular Mode)
    DMA2_Stream0->CR |= DMA_SxCR_CIRC;
    
    // MINC = 1: T? d?ng tang d?a ch? b? nh? (Memory Increment)
    DMA2_Stream0->CR |= DMA_SxCR_MINC;
    
    // PINC = 0: Không tang d?a ch? ngo?i vi (luôn d?c t? ADC1->DR)
    DMA2_Stream0->CR &= ~DMA_SxCR_PINC;
    
    // PSIZE[1:0] = 01: Kích thu?c d? li?u ngo?i vi = 16-bit (Half-word)
    DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;
    
    // MSIZE[1:0] = 01: Kích thu?c d? li?u b? nh? = 16-bit (Half-word)
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;
    
    // PL[1:0] = 10: Ð? uu tiên = High (Tùy ch?n)
    DMA2_Stream0->CR |= DMA_SxCR_PL_1;

    // 7. B?t Stream 0
    DMA2_Stream0->CR |= DMA_SxCR_EN;
}