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

#define WKUP_GPIO_PORT GPIOA
#define WKUP_GPIO_PIN  GPIO_PIN_0
#define KEY0_GPIO_PORT GPIOE
#define KEY0_GPIO_PIN  GPIO_PIN_4
#define KEY1_GPIO_PORT GPIOE
#define KEY1_GPIO_PIN  GPIO_PIN_3

#define LED_ON   GPIO_PIN_RESET
#define LED_OFF  GPIO_PIN_SET

#define BUTTON_ACTIVE_HIGH GPIO_PIN_SET
#define BUTTON_ACTIVE_LOW  GPIO_PIN_RESET

#define DEBOUNCE_MS 20u
#define BLINK_INTERVAL_MS 500u

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef enum
{
  APP_MODE_AUTO_BLINK = 0u,
  APP_MODE_MANUAL
} AppMode_t;

typedef struct
{
  GPIO_TypeDef *GPIOx;
  uint16_t GPIO_Pin;
  GPIO_PinState activeState;
  GPIO_PinState stableState;
  GPIO_PinState lastSample;
  uint32_t lastChangeTick;
  uint8_t pressLatched;
} Button_t;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void USER_GPIO_Init(void);
static void LED_Set(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t on);
static void Button_Init(Button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState activeState);
static uint8_t Button_Update(Button_t *button);

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
  /* USER CODE BEGIN 2 */
  USER_GPIO_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    AppMode_t appMode = APP_MODE_AUTO_BLINK;
    uint32_t lastBlinkTick = HAL_GetTick();
    uint8_t blinkPhase = 0u;
    uint8_t led0On = 1u;
    uint8_t led1On = 0u;
    Button_t wkupButton;
    Button_t key0Button;
    Button_t key1Button;

    Button_Init(&wkupButton, WKUP_GPIO_PORT, WKUP_GPIO_PIN, BUTTON_ACTIVE_HIGH);
    Button_Init(&key0Button, KEY0_GPIO_PORT, KEY0_GPIO_PIN, BUTTON_ACTIVE_LOW);
    Button_Init(&key1Button, KEY1_GPIO_PORT, KEY1_GPIO_PIN, BUTTON_ACTIVE_LOW);

    LED_Set(LED0_GPIO_PORT, LED0_GPIO_PIN, led0On);
    LED_Set(LED1_GPIO_PORT, LED1_GPIO_PIN, led1On);

    while (1)
    {
      uint32_t now = HAL_GetTick();

      if ((appMode == APP_MODE_AUTO_BLINK) && ((now - lastBlinkTick) >= BLINK_INTERVAL_MS))
      {
        lastBlinkTick = now;
        blinkPhase ^= 1u;
        led0On = (blinkPhase == 0u);
        led1On = (blinkPhase == 1u);
        LED_Set(LED0_GPIO_PORT, LED0_GPIO_PIN, led0On);
        LED_Set(LED1_GPIO_PORT, LED1_GPIO_PIN, led1On);
      }

      if (Button_Update(&wkupButton))
      {
        appMode = APP_MODE_MANUAL;
        led0On ^= 1u;
        LED_Set(LED0_GPIO_PORT, LED0_GPIO_PIN, led0On);
      }

      if (Button_Update(&key0Button))
      {
        appMode = APP_MODE_MANUAL;
        led1On ^= 1u;
        LED_Set(LED1_GPIO_PORT, LED1_GPIO_PIN, led1On);
      }

      if (Button_Update(&key1Button))
      {
        appMode = APP_MODE_MANUAL;
        led0On ^= 1u;
        led1On ^= 1u;
        LED_Set(LED0_GPIO_PORT, LED0_GPIO_PIN, led0On);
        LED_Set(LED1_GPIO_PORT, LED1_GPIO_PIN, led1On);
      }
    }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void USER_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, LED_OFF);
  HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, LED_OFF);

  GPIO_InitStruct.Pin = LED0_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED1_GPIO_PIN;
  HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Pin = WKUP_GPIO_PIN;
  HAL_GPIO_Init(WKUP_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Pin = KEY0_GPIO_PIN;
  HAL_GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = KEY1_GPIO_PIN;
  HAL_GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStruct);
}

static void LED_Set(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t on)
{
  HAL_GPIO_WritePin(GPIOx, GPIO_Pin, on ? LED_ON : LED_OFF);
}

static void Button_Init(Button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState activeState)
{
  GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

  button->GPIOx = GPIOx;
  button->GPIO_Pin = GPIO_Pin;
  button->activeState = activeState;
  button->stableState = pinState;
  button->lastSample = pinState;
  button->lastChangeTick = HAL_GetTick();
  button->pressLatched = (pinState == activeState);
}

static uint8_t Button_Update(Button_t *button)
{
  GPIO_PinState sample = HAL_GPIO_ReadPin(button->GPIOx, button->GPIO_Pin);
  uint32_t now = HAL_GetTick();

  if (sample != button->lastSample)
  {
    button->lastSample = sample;
    button->lastChangeTick = now;
  }

  if ((now - button->lastChangeTick) < DEBOUNCE_MS)
  {
    return 0u;
  }

  if (sample != button->stableState)
  {
    button->stableState = sample;

    if (button->stableState == button->activeState)
    {
      if (button->pressLatched == 0u)
      {
        button->pressLatched = 1u;
        return 1u;
      }
    }
    else
    {
      button->pressLatched = 0u;
    }
  }

  if (button->stableState != button->activeState)
  {
    button->pressLatched = 0u;
  }

  return 0u;
}
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
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
