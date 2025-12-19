/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Code Final - Fix lỗi động cơ không quay + Nút bấm Pull-down
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    GPIO_TypeDef* Port;
    uint16_t Pin;
} Led_TypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
Led_TypeDef leds[8] = {
    {GPIOA, GPIO_PIN_4}, {GPIOA, GPIO_PIN_5}, {GPIOA, GPIO_PIN_6}, {GPIOA, GPIO_PIN_7},
    {GPIOC, GPIO_PIN_4}, {GPIOC, GPIO_PIN_5}, {GPIOB, GPIO_PIN_0}, {GPIOB, GPIO_PIN_1}
};

int current_mode = 1;
uint8_t btn_prev = 0; // Logic Pull-down: Ban đầu mức 0

void Clear_All_Leds() {
    for(int i=0; i<8; i++) HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_RESET);
}

// Logic nút bấm cho mạch Pull-down (Nhấn nối VCC)
int Check_Change_Mode(void) {
    uint8_t btn_now = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);

    // Phát hiện sườn lên (0 -> 1)
    if (btn_now == 1 && btn_prev == 0) {
        HAL_Delay(50); // Chống rung
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == 1) {
            current_mode++;
            if (current_mode > 3) current_mode = 1;

            // Reset trạng thái
            Clear_All_Leds();
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);

            btn_prev = 1;
            return 1;
        }
    }
    if (btn_now == 0) btn_prev = 0;
    return 0;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  // Bắt đầu PWM
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      Check_Change_Mode();

      // --- MODE 1: LED LƯỢN SÓNG ---
      if (current_mode == 1) {
          for (int i = 0; i < 8; i++) {
              if(Check_Change_Mode()) break;
              HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_SET);
              HAL_Delay(100);
              HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_RESET);
          }
          for (int i = 6; i > 0; i--) {
              if(Check_Change_Mode()) break;
              HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_SET);
              HAL_Delay(100);
              HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_RESET);
          }
      }

      // --- MODE 2: LED NHÁY SO LE ---
      else if (current_mode == 2) {
          for(int i=0; i<8; i++) {
              if(i % 2 == 0) HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_SET);
              else           HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_RESET);
          }
          for(int d=0; d<50; d++) { HAL_Delay(10); if(Check_Change_Mode()) break; }

          if(current_mode != 2) continue;

          for(int i=0; i<8; i++) {
              if(i % 2 != 0) HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_SET);
              else           HAL_GPIO_WritePin(leds[i].Port, leds[i].Pin, GPIO_PIN_RESET);
          }
          for(int d=0; d<50; d++) { HAL_Delay(10); if(Check_Change_Mode()) break; }
      }

      // --- MODE 3: ĐỘNG CƠ CHẠY THẲNG ---
      else if (current_mode == 3) {
          Clear_All_Leds();
          int speed = 800; // Duty Cycle ~80%

          // Động cơ 1: IN1 PWM, IN2 = 0 -> Quay thuận trái
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);

          // Động cơ 2: IN3 PWM, IN4 = 0 -> Quay thuận
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed);

          HAL_Delay(100);
      }
    /* USER CODE END 3 */
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0}; // <--- Thêm khai báo GPIO

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 15;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) Error_Handler();

  // --- [QUAN TRỌNG] CẤU HÌNH CHÂN PWM THỦ CÔNG TẠI ĐÂY ĐỂ TRÁNH LỖI MẤT KẾT NỐI ---
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**TIM2 GPIO Configuration
  PA0     ------> TIM2_CH1
  PA1     ------> TIM2_CH2
  PA2     ------> TIM2_CH3
  PA3     ------> TIM2_CH4
  */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Chế độ Alternate Function
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2; // Gán chức năng Timer 2
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // --------------------------------------------------------------------------------
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  // Nút bấm Pull-down
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Các chân LED
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
