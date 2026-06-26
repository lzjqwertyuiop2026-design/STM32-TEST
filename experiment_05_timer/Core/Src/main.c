/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <stdbool.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED0_GPIO_PORT GPIOB
#define LED0_GPIO_PIN  GPIO_PIN_5
#define LED1_GPIO_PORT GPIOE
#define LED1_GPIO_PIN  GPIO_PIN_5

#define LED_ON  GPIO_PIN_RESET
#define LED_OFF GPIO_PIN_SET

#define TIM2_PWM_PERIOD              199U
#define TIM2_PWM_PULSE               50U
#define TIM2_UPDATES_PER_LED0_TOGGLE 20000U

#define TIM3_PWM_PERIOD              124U
#define TIM3_UPDATES_PER_LED1_TOGGLE 8000U
#define TIM3_BREATH_STEP_TICKS       80U

#define TIM5_INPUT_CAPTURE_CHANNEL   TIM_CHANNEL_1
#define TIM5_RISING_CAPTURE_CHANNEL  TIM_CHANNEL_2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
UART_HandleTypeDef huart1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;

static uint8_t usart1RxData;
static uint8_t led0LogicState = LED_OFF;
static bool led0BreathingEnabled;
static volatile bool tim5WaitingForRisingEdge;
static volatile bool tim5PulseCaptured;
static uint32_t tim2UpdateCounter;
static uint32_t tim3UpdateCounter;
static uint32_t tim3BreathTickCounter;
static volatile uint32_t lowPulseStartUs;
static volatile uint32_t lastLowPulseWidthUs;
static uint16_t led0BreathPwmCounter;
static uint16_t led0BreathPulse;
static int16_t led0BreathStep = 2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void USER_GPIO_Init(void);
static void USER_USART1_UART_Init(void);
static void USER_TIM2_Init(void);
static void USER_TIM3_Init(void);
static void USER_TIM5_Init(void);
static void USER_LED0_SetState(uint8_t pinState);
static void USER_LED0_ApplyGpioMode(void);
static void USER_SetLed0BreathingMode(bool enable);
static void USER_ProcessSerialCommand(uint8_t rxData);
static void USER_PrintStartupBanner(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void USER_LED0_SetState(uint8_t pinState)
{
  led0LogicState = pinState;
  if (!led0BreathingEnabled)
  {
    HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, pinState);
  }
}

static void USER_LED0_ApplyGpioMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = LED0_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, led0LogicState);
}

static void USER_SetLed0BreathingMode(bool enable)
{
  if (enable == led0BreathingEnabled)
  {
    return;
  }

  if (enable)
  {
    led0BreathPwmCounter = 0U;
    led0BreathPulse = 0U;
    led0BreathStep = 2;
    tim3BreathTickCounter = 0U;
    led0BreathingEnabled = true;
    printf("LED0 software breathing mode enabled on PB5.\r\n");
  }
  else
  {
    led0BreathPwmCounter = 0U;
    led0BreathPulse = 0U;
    led0BreathStep = 2;
    tim2UpdateCounter = TIM2_UPDATES_PER_LED0_TOGGLE - 1U;
    led0BreathingEnabled = false;
    USER_LED0_SetState(LED_OFF);
    printf("LED0 returned to TIM2 0.5 s toggle mode.\r\n");
  }
}

static void USER_ProcessSerialCommand(uint8_t rxData)
{
  switch (rxData)
  {
    case (uint8_t)'b':
    case (uint8_t)'B':
      USER_SetLed0BreathingMode(true);
      break;

    case (uint8_t)'n':
    case (uint8_t)'N':
      USER_SetLed0BreathingMode(false);
      break;

    case (uint8_t)'p':
    case (uint8_t)'P':
      if (tim5PulseCaptured)
      {
        printf("Latest low pulse width: %lu us\r\n", lastLowPulseWidthUs);
      }
      else
      {
        printf("No TIM5 pulse captured. Connect PB10 to PA0.\r\n");
      }
      break;

    case (uint8_t)'h':
    case (uint8_t)'H':
      USER_PrintStartupBanner();
      break;

    default:
      printf("Unknown command: %c\r\n", rxData);
      break;
  }
}

static void USER_PrintStartupBanner(void)
{
  printf("\r\nExperiment 5 ready.\r\n");
  printf("TIM2 CH3 on PB10 outputs 40 kHz PWM at 25%% duty.\r\n");
  printf("TIM2 update divider toggles LED0 every 0.5 s in normal mode.\r\n");
  printf("TIM3 update divider toggles LED1 every 1 s.\r\n");
  printf("TIM3 interrupt provides software breathing PWM on PB5 after command 'b'.\r\n");
  printf("TIM5 CH1 on PA0 captures low pulse width and reports it over USART1.\r\n");
  printf("Commands: b=breathing, n=normal LED0, p=print last pulse width, h=help.\r\n");
  printf("Connect PB10 to PA0 with a jumper wire for the easiest self-test.\r\n");
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  USER_GPIO_Init();
  USER_USART1_UART_Init();
  USER_TIM2_Init();
  USER_TIM3_Init();
  USER_TIM5_Init();

  USER_PrintStartupBanner();

  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_IC_Start_IT(&htim5, TIM5_INPUT_CAPTURE_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_IC_Start_IT(&htim5, TIM5_RISING_CAPTURE_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_UART_Receive_IT(&huart1, &usart1RxData, 1);
  /* USER CODE END 2 */

  while (1)
  {
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

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

/* USER CODE BEGIN 4 */
static void USER_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  __HAL_AFIO_REMAP_SWJ_NOJTAG();
  __HAL_AFIO_REMAP_TIM2_PARTIAL_2();

  USER_LED0_ApplyGpioMode();
  HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, LED_OFF);

  GPIO_InitStruct.Pin = LED1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void USER_USART1_UART_Init(void)
{
  __HAL_RCC_USART1_CLK_ENABLE();

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

static void USER_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIM2_PWM_PERIOD;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

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
  sConfigOC.Pulse = TIM2_PWM_PULSE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
}

static void USER_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = TIM3_PWM_PERIOD;
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

  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void USER_TIM5_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 7;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 0xFFFFFFFFU;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM5_INPUT_CAPTURE_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM5_RISING_CAPTURE_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }
}

int __io_putchar(int ch)
{
  uint8_t data = (uint8_t)ch;
  HAL_UART_Transmit(&huart1, &data, 1, HAL_MAX_DELAY);
  return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    USER_ProcessSerialCommand(usart1RxData);
    HAL_UART_Receive_IT(&huart1, &usart1RxData, 1);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    USER_TIM2_UpdateIRQ();
  }
  else if (htim->Instance == TIM3)
  {
    USER_TIM3_UpdateIRQ();
  }
}

void USER_TIM2_UpdateIRQ(void)
{
  tim2UpdateCounter++;
  if (tim2UpdateCounter >= TIM2_UPDATES_PER_LED0_TOGGLE)
  {
    tim2UpdateCounter = 0;
    if (!led0BreathingEnabled)
    {
      USER_LED0_SetState((led0LogicState == LED_ON) ? LED_OFF : LED_ON);
    }
  }
}

void USER_TIM3_UpdateIRQ(void)
{
  tim3UpdateCounter++;
  if (tim3UpdateCounter >= TIM3_UPDATES_PER_LED1_TOGGLE)
  {
    tim3UpdateCounter = 0;
    HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN);
  }

  if (led0BreathingEnabled)
  {
    led0BreathPwmCounter++;
    if (led0BreathPwmCounter > TIM3_PWM_PERIOD)
    {
      led0BreathPwmCounter = 0U;
    }

    HAL_GPIO_WritePin(LED0_GPIO_PORT,
                      LED0_GPIO_PIN,
                      (led0BreathPwmCounter < led0BreathPulse) ? LED_ON : LED_OFF);

    tim3BreathTickCounter++;
    if (tim3BreathTickCounter >= TIM3_BREATH_STEP_TICKS)
    {
      int32_t nextPulse;

      tim3BreathTickCounter = 0;

      if ((led0BreathStep > 0) && (led0BreathPulse >= TIM3_PWM_PERIOD))
      {
        led0BreathStep = -2;
      }
      else if ((led0BreathStep < 0) && (led0BreathPulse == 0U))
      {
        led0BreathStep = 2;
      }

      nextPulse = (int32_t)led0BreathPulse + led0BreathStep;
      if (nextPulse < 0)
      {
        nextPulse = 0;
      }
      else if (nextPulse > TIM3_PWM_PERIOD)
      {
        nextPulse = TIM3_PWM_PERIOD;
      }
      led0BreathPulse = (uint16_t)nextPulse;
    }
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM5)
  {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      lowPulseStartUs = HAL_TIM_ReadCapturedValue(htim, TIM5_INPUT_CAPTURE_CHANNEL);
      tim5WaitingForRisingEdge = true;
    }
    else if ((htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) && tim5WaitingForRisingEdge)
    {
      uint32_t capturedUs = HAL_TIM_ReadCapturedValue(htim, TIM5_RISING_CAPTURE_CHANNEL);

      lastLowPulseWidthUs = capturedUs - lowPulseStartUs;
      tim5PulseCaptured = true;
      tim5WaitingForRisingEdge = false;
    }
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
}
#endif /* USE_FULL_ASSERT */
