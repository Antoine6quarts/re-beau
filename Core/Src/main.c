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
#include <stdio.h>

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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _close(int fd) { return 1; }
int _fstat(int fd) { return 1; }
int _getpid(int fd) { return 1; }
int _isatty(int fd) { return 1; }
int _kill(int fd) { return 1; }
int _read(int fd, char* ptr, int len) { return 0; }
int _lseek(int fd) { return 1; }
void _exit(int fd) { while(1); }
int _sbrk(int fd) { return 1; }

int _write(int fd, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

volatile int process = 0;
void kalman_process();

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == IMU_INT_Pin)
    {
        if (process == 0)
            return;
        kalman_process();
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  printf("%s", "BBBBBB\n");
  HAL_Delay(300);

  // Init the I2C device
  HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c1, IMU_ADR_WRITE, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to init device!\n");

  // Set the gyroscope scale range to +/-250 deg/s
  // Register 27 (see register map p14)
  uint8_t data = 0;
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 27, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to setup gyroscope range!\n");
  
  // Set the accelerometer scale range to +/-2G deg/s
  // Register 28 (see register map p15)
  data = 0;
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 28, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to setup accelerometer range!\n");
  
  // Exit sleep mode / Wake up the device. We won't receive any data when it is in sleep mode.
  // Register 107 (see register map p40)
  data = 0;
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 107, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to wake up the device!\n");
  
  data = 0;
  ret = HAL_I2C_Mem_Read(&hi2c1, IMU_ADR_READ, 26, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to read device config!\n");
  data = (data & 0b11111000) | 0b0001;
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 26, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to write device config!\n");

  data = 19;
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 25, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to set sample freq!\n");

  data = 0;
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 55, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to setup interupt!\n");

  data = 0b0001;
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 56, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to setup interupt!\n");

  data = (1 << 6) | (1 << 2);
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 106, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to setup FIFO!\n");

  data = (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3);
  ret = HAL_I2C_Mem_Write(&hi2c1, IMU_ADR_WRITE, 35, 1, &data, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) printf("Failed to setup FIFO!\n");

  printf("MPU6050 Initialized correctly\n");
  // Little delay in case the MPU need to do some work before being able to receive data
  HAL_Delay(15);

  process = 1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(75);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IMU_INT_Pin */
  GPIO_InitStruct.Pin = IMU_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

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
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1)
    {
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
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
