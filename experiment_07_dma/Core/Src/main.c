/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : DMA USART transmit experiment
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define LED0_GPIO_PORT GPIOB
#define LED0_GPIO_PIN  GPIO_PIN_5
#define KEY0_GPIO_PORT GPIOE
#define KEY0_GPIO_PIN  GPIO_PIN_4

#define LED_ON  GPIO_PIN_RESET
#define LED_OFF GPIO_PIN_SET

#define DMA_TX_BUFFER_SIZE 3000U
#define LED_TOGGLE_INTERVAL_MS 500U
#define KEY_DEBOUNCE_MS 50U
/* USER CODE END PD */

/* USER CODE BEGIN PV */
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

static uint8_t dmaTxBuffer[DMA_TX_BUFFER_SIZE];
static volatile bool dmaTxBusy;
static uint32_t lastLedToggleTick;
static uint32_t keyPressedTick;
static GPIO_PinState lastKeyState = GPIO_PIN_SET;
/* USER CODE END PV */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void USER_GPIO_Init(void);
static void USER_USART1_UART_Init(void);
static void USER_DMA_Init(void);
static void USER_FillDmaBuffer(void);
static void USER_StartDmaTransmit(void);
static void USER_ProcessKey(void);
static void USER_PrintStartupBanner(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
  uint8_t data = (uint8_t)ch;
  HAL_UART_Transmit(&huart1, &data, 1, HAL_MAX_DELAY);
  return ch;
}

static void USER_PrintStartupBanner(void)
{
  printf("\r\nExperiment 7 DMA ready.\r\n");
  printf("KEY0 on PE4 starts one USART1 DMA transmission.\r\n");
  printf("DMA buffer length: %u bytes.\r\n", (unsigned int)DMA_TX_BUFFER_SIZE);
  printf("USART1: 115200 8N1.\r\n");
}

static void USER_FillDmaBuffer(void)
{
  uint8_t letter = (uint8_t)'a';

  for (uint32_t i = 0U; i < DMA_TX_BUFFER_SIZE; i++)
  {
    if ((i & 1U) == 0U)
    {
      dmaTxBuffer[i] = letter;
      letter++;
      if (letter > (uint8_t)'z')
      {
        letter = (uint8_t)'a';
      }
    }
    else
    {
      dmaTxBuffer[i] = (uint8_t)' ';
    }
  }
}

static void USER_StartDmaTransmit(void)
{
  if (dmaTxBusy)
  {
    printf("\r\nDMA busy, please wait.\r\n");
    return;
  }

  dmaTxBusy = true;
  printf("\r\nDMA USART1 TX start, %u bytes:\r\n", (unsigned int)DMA_TX_BUFFER_SIZE);
  if (HAL_UART_Transmit_DMA(&huart1, dmaTxBuffer, DMA_TX_BUFFER_SIZE) != HAL_OK)
  {
    dmaTxBusy = false;
    Error_Handler();
  }
}

static void USER_ProcessKey(void)
{
  GPIO_PinState keyState = HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN);
  uint32_t now = HAL_GetTick();

  if ((lastKeyState == GPIO_PIN_SET) && (keyState == GPIO_PIN_RESET))
  {
    keyPressedTick = now;
  }
  else if ((lastKeyState == GPIO_PIN_RESET) && (keyState == GPIO_PIN_RESET))
  {
    if ((now - keyPressedTick) >= KEY_DEBOUNCE_MS)
    {
      USER_StartDmaTransmit();
      keyPressedTick = now + 100000U;
    }
  }
  else if (keyState == GPIO_PIN_SET)
  {
    keyPressedTick = now;
  }

  lastKeyState = keyState;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    dmaTxBusy = false;
    printf("\r\nDMA USART1 TX complete.\r\n");
  }
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  USER_GPIO_Init();
  USER_USART1_UART_Init();
  USER_DMA_Init();
  USER_FillDmaBuffer();
  USER_PrintStartupBanner();
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN WHILE */
    uint32_t now = HAL_GetTick();

    if ((now - lastLedToggleTick) >= LED_TOGGLE_INTERVAL_MS)
    {
      lastLedToggleTick = now;
      HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
    }

    USER_ProcessKey();
    HAL_Delay(5U);
    /* USER CODE END WHILE */
  }
}

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

static void MX_GPIO_Init(void)
{
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

  HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, LED_OFF);
  GPIO_InitStruct.Pin = LED0_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = KEY0_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
}

static void USER_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_usart1_tx.Instance = DMA1_Channel4;
  hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart1_tx.Init.Mode = DMA_NORMAL;
  hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
  if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);

  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

  HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

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
#endif
/* USER CODE END 4 */
