/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file               : main.c
  * @brief              : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>  // Để dùng sprintf
#include <string.h> // Để dùng strlen

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUFFER_SIZE 50 // number of samples to collect

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint16_t adc_buffer[ADC_BUFFER_SIZE]; // buffer to hold ADC samples
volatile uint8_t dma_transfer_complete = 0; // flag to indicate DMA transfer completion

// <<< SỬA LỖI (Thêm biến cho Systick) >>>
volatile uint16_t g_latest_avg_val = 0; // Biến "an toàn" để in
volatile uint32_t g_systick_counter = 0; // Bộ đếm cho Systick
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void My_Clock_Init_HSI(void);
void My_GPIO_Init(void); 
void My_DMA_Init(void);
void My_ADC_Init(void);
void MY_NVIC_Init(void);
void My_UART_GPIO_Init(void);
void My_UART_Init(void);
void UART_SendChar(char c);     
void UART_SendString(char* s);  
void UART_SendNumber(uint16_t num);
void DMA2_Stream0_IRQHandler(void);
void My_SysTick_Init(void); // <<< SỬA LỖI (Thêm prototype) >>>
void My_IR_LED_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  // <<< SỬA LỖI (Gọi Clock NGAY LẬP TỨC) >>>
  My_Clock_Init_HSI();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  //My_Clock_Init_HSI(); // <<< SỬA LỖI (Đã di chuyển lên USER CODE 1) >>>
  My_GPIO_Init();
  My_DMA_Init();
  My_ADC_Init();
  MY_NVIC_Init();
  My_UART_GPIO_Init();
  My_UART_Init();
  My_SysTick_Init(); // <<< SỬA LỖI (Gọi hàm Init Systick) >>>
  My_IR_LED_Init();
  // Start ADC Conversion
  ADC1->CR2 |= ADC_CR2_SWSTART;
  UART_SendString("Hello!\r\n");
  UART_SendString("Testing...\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // Tắt LED (Ghi 1)
    //GPIOC->BSRR = GPIO_BSRR_BR_13;
    // Gửi 1 (3.3V) tới PC13 để BẬT N-MOSFET
    GPIOC->BSRR = GPIO_BSRR_BS_13; //BS mean Set, BR mean Reset
    // <<< SỬA LỖI (Tách biệt logic tính toán và logic in) >>>
    if (dma_transfer_complete == 1)
    {
      // 1. XÓA CỜ NGAY LẬP TỨC
      dma_transfer_complete = 0; 

      // 2. Chỉ tính toán và cập nhật biến "an toàn"
      // (Không cần copy buffer, vì chúng ta chỉ cập nhật 1 biến)
      uint32_t sum = 0;
      for(int i=0; i < ADC_BUFFER_SIZE; i++)
      {
        sum += adc_buffer[i];
      }
      
      // 3. Cập nhật biến
      // Ngắt SysTick sẽ tự động lấy biến này để in
      g_latest_avg_val = sum / ADC_BUFFER_SIZE;
      
      // 4. TUYỆT ĐỐI KHÔNG GỌI UART_SendNumber() TẠI ĐÂY
    }
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief Cấu hình SysTick timer 1ms (Giả định HCLK 168MHz)
  * @retval None
  */
// <<< SỬA LỖI (Thêm hàm Init Systick) >>>
void My_SysTick_Init(void)
{
  // Clock của bạn là 168MHz (từ hàm My_Clock_Init_HSI)
  // 168.000.000 / 1000 (cho 1ms) = 168000
  SysTick_Config(168000); 
}

/**
  * @brief Hàm xử lý ngắt SysTick (Tên hàm là bắt buộc)
  * @retval None
  */
// <<< SỬA LỖI (Thêm hàm Handler Systick) >>>
void SysTick_Handler(void)
{
  g_systick_counter++; // Tăng bộ đếm mỗi 1ms

  // In 10 lần 1 giây (mỗi 100ms)
  if (g_systick_counter >= 100)
  {
    g_systick_counter = 0; // Reset bộ đếm
    
    // In giá trị trung bình "an toàn"
    UART_SendNumber(g_latest_avg_val);
  }
}

/**
  * @brief Cấu hình PC2 
  * @retval None
  */
void My_GPIO_Init(void)
{
  // 1. Enable GPIO clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  // 2. Configure PC2 as an analog input
  GPIOC->MODER |= GPIO_MODER_MODE2;

  // 3. Disable pull-up/pull-down resistors for PC2
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD2; 
}

/**
  * @brief Cấu hình DMA2 Stream 0 cho ADC1
  * @retval None
  */
void My_DMA_Init(void)
{
  // 1. Enable DMA2 clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

  // 2. Configure DMA2 Stream 0 for ADC1
  DMA2_Stream0->CR &= ~DMA_SxCR_EN; 
  while (DMA2_Stream0->CR & DMA_SxCR_EN);

  // 3. SET CHANNEL 0 FOR STREAM 0
  DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL; 

  // 4. SET ADDRESS OF PERIPHERAL (ADC1 DATA REGISTER)
  DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;

  // 5. SET ADDRESS OF MEMORY (adc_buffer)
  DMA2_Stream0->M0AR = (uint32_t)adc_buffer;

  // 6. SET NUMBER OF DATA ITEMS TO TRANSFER
  DMA2_Stream0->NDTR = ADC_BUFFER_SIZE;

  // 7. CONFIGURE CONTROL REGISTER
  DMA2_Stream0->CR |= DMA_SxCR_PL_1; 
  DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0; 
  DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0; 
  DMA2_Stream0->CR |= DMA_SxCR_MINC; 
  DMA2_Stream0->CR |= DMA_SxCR_CIRC;    

  DMA2_Stream0->CR |= DMA_SxCR_TCIE;
  // 8. ENABLE DMA STREAM
  DMA2_Stream0->CR |= DMA_SxCR_EN;
}

void My_ADC_Init(void)
{
  // 1. Enable ADC1 clock
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // set bit adc1 lên 1 

  // 2. Configure ADC1 ( CR1 )
  ADC1->CR1 |= ADC_CR1_SCAN; // adc sẽ thực hiện scan nhiều kênh (nếu có)

  // 3. CONFIGURE ADC1 ( CR2 )
  ADC1->CR2 |= ADC_CR2_CONT; // adc sẽ thực hiện liên tục, cr2 register
  ADC1->CR2 |= ADC_CR2_DMA;  // enable dma for adc
  ADC1->CR2 |= ADC_CR2_DDS;  // dma sẽ request liên tục, dds is dma disable selection
  
  // 4. CONFIGURE ADC1 REGULAR SEQUENCE ( SQR3 )
  ADC1->SQR1 &= ~ADC_SQR1_L; // 0B0000 = 1 CONVERSION
  // (Chúng ta chỉ đọc 1 kênh duy nhất)
  // sqr1 register is for length of sequence

  // 5. CONFIGURE SEQUANCE REGISTER 3 ( SQR3 )
  ADC1->SQR3 = (12 << ADC_SQR3_SQ1_Pos); // 

  // 6. CONFIGURE SAMPLING TIME ( SMPR1 )
  //ADC1->SMPR1 |= (0b110 << ADC_SMPR1_SMP12_Pos); 
  ADC1->SMPR1 |= (0b111 << ADC_SMPR1_SMP12_Pos);
  // 7. ENABLE ADC1
  ADC1->CR2 |= ADC_CR2_ADON;

  // 8.  WAIT FOR ADC1 TO STABILIZE
  for (volatile uint32_t i = 0; i < 10000; i++);
}

void MY_NVIC_Init(void)
{
  // ENABLE DMA2 STREAM0 INTERRUPT IN NVIC
  NVIC_SetPriority(DMA2_Stream0_IRQn, 1);
  NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

void DMA2_Stream0_IRQHandler(void)
{
  // 1. check TRANSFER COMPLETE FLAG
  if (DMA2->LISR & DMA_LISR_TCIF0)
  {
    // CLEAR THE TRANSFER COMPLETE FLAG
    DMA2->LIFCR = DMA_LIFCR_CTCIF0;

    // SET THE TRANSFER COMPLETE FLAG
    dma_transfer_complete = 1;
  }
}

void My_UART_GPIO_Init(void)
{
  // 1. Cấp clock cho GPIOA (chứa PA9)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // 2. Chuyển PA9 sang chế độ Alternate Function (AF)
  GPIOA->MODER &= ~GPIO_MODER_MODE9_0;
  GPIOA->MODER |= GPIO_MODER_MODE9_1;

  // 3. Đặt tốc độ (Speed) cho chân (ví dụ: High Speed)
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED9; 

  // 4. Cấu hình Alternate Function là AF7 (cho USART1)
  GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9; 
  GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos); 
}

void My_UART_Init(void)
{
  // 1. Cấp clock cho USART1
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  // 2. Cấu hình Control Register 1 (CR1)
  USART1->CR1 |= USART_CR1_TE;  

  // 3. Cấu hình Baud Rate (Giả định PCLK2 = 84MHz)
  
  // <<< SỬA LỖI (Sửa comment cho đúng) >>>
  USART1->BRR = 0x2D9; // (BRR cho 84MHz @ 115200 baud)

  // 4. Bật USART1
  USART1->CR1 |= USART_CR1_UE;  
}

void UART_SendChar(char c)
{
  // 1. Chờ cho đến khi thanh ghi Transmit Data (TDR) rảnh
  while( !(USART1->SR & USART_SR_TXE) );
  
  // 2. Ghi dữ liệu vào Data Register (DR)
  USART1->DR = c;
}

/**
  * @brief Gửi 1 chuỗi (string) qua UART1
  * @retval None
  */
void UART_SendString(char* s)
{
  while(*s)
  {
    UART_SendChar(*s++);
  }
}

/**
  * @brief Gửi 1 số (uint16_t) qua UART1 (chuyển sang string)
  * @retval None
  */
void UART_SendNumber(uint16_t num)
{
  char buffer[20]; 
  
  sprintf(buffer, "%u\r\n", num);
  
  UART_SendString(buffer);
}

void My_Clock_Init_HSI(void)
{
  // 1. Bật HSI (đã bật mặc định, nhưng nên làm) và chờ
  RCC->CR |= RCC_CR_HSION;
  while(!(RCC->CR & RCC_CR_HSIRDY));

  // 2. Bật clock cho Power Controller và set Voltage Scale 1
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_VOS; // Scale 1 mode (cần cho > 144MHz)

  // 3. Cấu hình Prescaler cho các bus
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB Prescaler = 1 (HCLK = 168MHz)
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; // APB2 Prescaler = 2 (PCLK2 = 84MHz)
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4; // APB1 Prescaler = 4 (PCLK1 = 42MHz)

  // 4. Cấu hình PLL (M=16, N=336, P=2)
  // (16MHz / M=16) * N=336 / P=2 = 168MHz
  RCC->PLLCFGR = (16 << 0) | (336 << 6) | (0 << 16); 
  RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC; // Chọn nguồn là HSI

  // 5. Bật PLL và chờ nó khóa
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY));

  // 6. CẤU HÌNH FLASH (RẤT QUAN TRỌNG)
  FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

  // 7. Chuyển System Clock (SYSCLK) sang dùng PLL
  RCC->CFGR &= ~RCC_CFGR_SW; 
  RCC->CFGR |= RCC_CFGR_SW_PLL; 

  // 8. Chờ đến khi hệ thống xác nhận đã chuyển sang PLL
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void My_IR_LED_Init(void)
{
  // 1. Cấp clock cho GPIOC
  // (Hàm My_GPIO_Init đã làm, nhưng làm lại cho chắc)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  // 2. Chuyển PC13 sang chế độ General purpose output (0b01)
  // Thanh ghi MODER, set 2 bit 26, 27 (MODE13)
  GPIOC->MODER |= GPIO_MODER_MODE13_0; 
  GPIOC->MODER &= ~GPIO_MODER_MODE13_1;

  // 3. Cấu hình Output push-pull (0b0)
  // Thanh ghi OTYPER, bit 13
  GPIOC->OTYPER &= ~GPIO_OTYPER_OT_13;

  // 4. Cấu hình Tốc độ (ví dụ: Low speed)
  // Thanh ghi OSPEEDR, 2 bit 26, 27 (OSPEED13) về 0b00
  GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED13;
}
  /* USER CODE END 4 */