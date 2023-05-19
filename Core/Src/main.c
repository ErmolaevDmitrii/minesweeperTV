/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t line = 0;

uint8_t image [5750] = {0};
gdi_handle handle = {image, 23 * 8, 250};
uint8_t game_field[81] = {0};

void delay_us (uint16_t us)
{
  //HAL_GPIO_TogglePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin);
  //__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
  __HAL_TIM_SET_COUNTER(&htim3, 0);
	while (htim3.Instance->CNT < us) ;
  //HAL_GPIO_TogglePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin);
}

typedef struct {
    uint32_t time_irq;
    uint8_t flag_irq;
    uint8_t flag_exec;
    uint16_t GPIO_pin;
    IRQn_Type GPIO_EXTI_line;
    void (*action) (game_handle *);
} button_handle;

button_handle buttons[] = {
    {0, 0, 0, DOWN_BTN_Pin , DOWN_BTN_EXTI_IRQn , game_player_move_down },
    {0, 0, 0, UP_BTN_Pin   , UP_BTN_EXTI_IRQn   , game_player_move_up   },
    {0, 0, 0, LEFT_BTN_Pin , LEFT_BTN_EXTI_IRQn , game_player_move_left },
    {0, 0, 0, RIGHT_BTN_Pin, RIGHT_BTN_EXTI_IRQn, game_player_move_right},
    {0, 0, 0, FLAG_BTN_Pin , FLAG_BTN_EXTI_IRQn , game_player_put_flag  },
    {0, 0, 0, OPEN_BTN_Pin , OPEN_BTN_EXTI_IRQn , game_player_open_cell },
};

/*
uint32_t time1_irq = 0;
uint8_t flag1_irq = 0;
uint8_t flag1_exec = 0;

uint32_t time2_irq = 0;
uint8_t flag2_irq = 0;
uint8_t flag2_exec = 0;
*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    for(uint8_t i = 0; i < (sizeof(buttons) / sizeof(button_handle)); ++i) {
        if(buttons[i].GPIO_pin == GPIO_Pin) {
            buttons[i].flag_exec = 0;
            buttons[i].flag_irq = 1;
            buttons[i].time_irq = HAL_GetTick();
            HAL_NVIC_DisableIRQ(buttons[i].GPIO_EXTI_line);
            break;
        }
    }

    return;
/*
        if(GPIO_Pin == GPIO_PIN_2)
        {
            flag2_exec = 0;
            flag2_irq = 1;
            time2_irq = HAL_GetTick();
            HAL_NVIC_DisableIRQ(EXTI2_TSC_IRQn);
        }

        if(GPIO_Pin == GPIO_PIN_4)
        {
            flag1_exec = 0;
            flag1_irq = 1;
            time1_irq = HAL_GetTick();
            HAL_NVIC_DisableIRQ(EXTI4_IRQn);
        }
*/
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
        if(htim->Instance == TIM1) //check if the interrupt comes from TIM1
        {

            HAL_GPIO_WritePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin, 0);

            if(line == 313) {
                delay_us(28);
                HAL_GPIO_WritePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin, 1);
                delay_us(4);
                HAL_GPIO_WritePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin, 0);
                delay_us(28);
                HAL_GPIO_WritePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin, 1);
                delay_us(3);
                __HAL_TIM_SET_COUNTER(&htim1, 0);
            }
            delay_us(4);
            HAL_GPIO_WritePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin, 1);
            delay_us(7);

            uint8_t *line_pointer = image + (line - 40) * 23;

            if(line >= 40 && line < 290) {
                if(HAL_SPI_Transmit_DMA(&hspi1, line_pointer, 23)
                  != HAL_OK) {
                  while(1) {
                      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
                      break;
                  }
                }
            }

            if(line == 313) {
                line = 0;
                return;
            }
            ++line;
        }
}

void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    for(int i = y; i < y + height; ++i) {
        for(int j = x; j < x + width; ++j) {
            if(j / 8 == 19) {
              continue;
            }
            image[i * 23 + j / 8] |= (1 << (j % 8));
        }
    }
}

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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_Base_Start_IT(&htim1);

  /*draw_picture(68, 0, 16, 16, one, &handle);
  draw_picture(68, 16, 16, 16, two, &handle);
  draw_picture(68, 32, 16, 16, three, &handle);
  draw_picture(68, 48, 16, 16, four, &handle);
  draw_picture(68, 60, 16, 16, five, &handle);
  draw_picture(68, 76, 16, 16, six, &handle);
  draw_picture(68, 92, 16, 16, seven, &handle);
  draw_picture(68, 92, 16, 16, eight, &handle);
  draw_picture(68, 108, 16, 16, flag, &handle);
  draw_picture(68, 124, 16, 16, bomb, &handle);
  draw_picture(68, 140, 16, 16, not_opened_cell, &handle);
  //draw_picture(68, 156, 16, 16, selected_cell, &handle);
  */

  game_handle game = {9, 9, game_field, &handle, };
  game_start(&game);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //uint16_t i = 0, j = 0;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    button_handle *temp;

    for(uint8_t i = 0; i < (sizeof(buttons) / sizeof(button_handle)); ++i) {
        temp = buttons + i;

        if(temp->flag_irq) {

            if(!temp->flag_exec) {
                temp->action(&game);
                temp->flag_exec = 1;
            }

            if((HAL_GetTick() - temp->time_irq) > 150) {
                __HAL_GPIO_EXTI_CLEAR_IT(temp->GPIO_pin);
                NVIC_ClearPendingIRQ(temp->GPIO_EXTI_line);
                HAL_NVIC_EnableIRQ(temp->GPIO_EXTI_line);
                temp->flag_irq = 0;
            }
        }
    }
    /*if(flag1_irq && (HAL_GetTick() - time1_irq) > 100)
    {
        if(!flag1_exec) {
            if(i == 0) {
              set_pixel(0, j, 1, &handle);
              ++i;
            }

            else if(i < 23 * 8) {
              set_pixel(i, j, 1, &handle);
              set_pixel(i - 1, j, 0, &handle);
              ++i;
            }

            else if(i >= 23 * 8) {
              i = 0;
            }
            flag1_exec = 1;
        }
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);  // очищаем бит EXTI_PR
        NVIC_ClearPendingIRQ(EXTI4_IRQn); // очищаем бит NVIC_ICPRx
        HAL_NVIC_EnableIRQ(EXTI4_IRQn);   // включаем внешнее прерывание
        flag1_irq = 0;
    }

    if(flag2_irq && (HAL_GetTick() - time2_irq) > 100)
    {
        if(!flag2_exec) {
            if(j == 0) {
              set_pixel(i, 0, 1, &handle);
              ++j;
            }

            else if(j < 250) {
              set_pixel(i, j, 1, &handle);
              set_pixel(i, j-1, 0, &handle);
              ++j;
            }

            else if(j >= 250) {
              j = 0;
            }
            flag2_exec = 1;
        }
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);  // очищаем бит EXTI_PR
        NVIC_ClearPendingIRQ(EXTI2_TSC_IRQn); // очищаем бит NVIC_ICPRx
        HAL_NVIC_EnableIRQ(EXTI2_TSC_IRQn);   // включаем внешнее прерывание
        flag2_irq = 0;
    }
    */

      //image[23*249 + 10]++;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_TIM34;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 2303;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TIM_TEST_GPIO_Port, TIM_TEST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OPEN_BTN_Pin FLAG_BTN_Pin */
  GPIO_InitStruct.Pin = OPEN_BTN_Pin|FLAG_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TIM_TEST_Pin */
  GPIO_InitStruct.Pin = TIM_TEST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TIM_TEST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : STLK_RX_Pin STLK_TX_Pin */
  GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RIGHT_BTN_Pin LEFT_BTN_Pin UP_BTN_Pin DOWN_BTN_Pin */
  GPIO_InitStruct.Pin = RIGHT_BTN_Pin|LEFT_BTN_Pin|UP_BTN_Pin|DOWN_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_TSC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_TSC_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    HAL_Delay(500);
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    HAL_Delay(500);
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
