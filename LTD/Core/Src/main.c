/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

#include "DHT11.h"
#include "scheduler.h"
#include "i2c-lcd.h"
#include "button.h"
#include "fan.h"
#include "heator.h"
#include "heatpump.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ONE_SECOND 100
#define TIME_OUT_DEVICE 5
#define TIME_OUT_DHT 20
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

//TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

enum mode{normal, adjustTemp, adjustHumid, adjustPeriod, simulation};
enum mode currentMode = normal;

enum choices{temp,humid};
enum choices currentChoice = temp;

uint8_t maxTemp = 40, adjustedTemp;
uint8_t maxHumid = 70, adjustedHumid;
uint8_t period = 5, adjustedPeriod;
uint8_t flag = 0;

DHT11_data result;

uint8_t s_temp, s_tempAdjusted;
uint8_t s_humid, s_humidAdjusted;
uint8_t data[10];

HAL_StatusTypeDef globalTimeout = HAL_OK;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

void printValue(uint8_t);
void printString(char* input);

void getTempAndHumid(void);

void displayLCD(void);
void displayChangedMaxTemp(uint8_t input);
void displayChangedMaxHum(uint8_t input);
void displayChangedPeriod(uint8_t input);
void displayMode(uint8_t input);
void displayTemp(uint8_t input);
void displayHumid(uint8_t input);
void displayTimeout(void);
void checkInformation(void);

void heator(void);
void heatpump(void);

void normalRun(void);

void changeMode(uint8_t inputMode);

void decideTempOrHumid(void);

void mainFSM(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void printValue(uint8_t val) {
	char uart_buf[50];
	int uart_buf_len;
	uart_buf_len = sprintf(uart_buf, "%u\r\n", val);
	HAL_UART_Transmit(&huart2, (uint8_t*) uart_buf, uart_buf_len, 100);
}

void printString(char* input) {
	char uart_buf[50];
	int uart_buf_len;
	uart_buf_len = sprintf(uart_buf, "%s\r\n", input);
	HAL_UART_Transmit(&huart2, (uint8_t*) uart_buf, uart_buf_len, 100);
}

void getTempAndHumid(void) {
	if (currentMode!=simulation) {
		if (getDHT11Data(&result,TIME_OUT_DHT) == HAL_TIMEOUT) {
			globalTimeout = HAL_TIMEOUT;
			return;
		}
	}
	else {
		result.temperature[0] = s_temp;
		result.humidity[0] = s_humid;
	}
	displayLCD();
}

void displayLCD(void) {
	char temp[3];
	char hum[3];

//	uint8_t t,h;
//	if (currentMode!=simulation) {
//		t = result.temperature[0];
//		h = result.humidity[0];
//	}
//	else {
//		t = s_temp;
//		h = s_humid;
//	}

	temp[0] = result.temperature[0] / 10 + 48;
	temp[1] = result.temperature[0] % 10 + 48;
	temp[2] = '\0';

	hum[0] = result.humidity[0] / 10 + 48;
	hum[1] = result.humidity[0] % 10 + 48;
	hum[2] = '\0';

	lcd_put_cur(0, 2);
	lcd_send_string(temp);
	lcd_put_cur(0, 8);
	lcd_send_string(hum);
}

void displayChangedMaxTemp(uint8_t input) {
//	char uart_buf[50];
//	int uart_buf_len;
//	uart_buf_len = sprintf(uart_buf, "\033[2J%s%u    \r", "Adjust Maximum Temperature: ", input);
//	HAL_UART_Transmit(&huart2, (uint8_t*) uart_buf, uart_buf_len, 100);
	char temp[3];
	temp[0] = input / 10 + 48;
	temp[1] = input % 10 + 48;
	temp[2] = '\0';
	lcd_put_cur(1, 2);
	lcd_send_string(temp);
}
void displayChangedMaxHum(uint8_t input) {
//	char uart_buf[50];
//	int uart_buf_len;
//	uart_buf_len = sprintf(uart_buf, "\033[2J%s%u    \r", "Adjust Maximum Humidity: ", input);
//	HAL_UART_Transmit(&huart2, (uint8_t*) uart_buf, uart_buf_len, 100);
	char humid[3];
	humid[0] = input / 10 + 48;
	humid[1] = input % 10 + 48;
	humid[2] = '\0';
	lcd_put_cur(1, 8);
	lcd_send_string(humid);
}

void displayChangedPeriod(uint8_t input) {
//	char uart_buf[50];
//	int uart_buf_len;
//	uart_buf_len = sprintf(uart_buf, "\033[2J%s%u    \r", "Adjust Turn On Period: ", input);
//	HAL_UART_Transmit(&huart2, (uint8_t*) uart_buf, uart_buf_len, 100);
	char period[3];
	period[0] = input / 10 + 48;
	period[1] = input % 10 + 48;
	period[2] = '\0';
	lcd_put_cur(1, 14);
	lcd_send_string(period);
}

void displayTemp(uint8_t input) {
	char temp[3];
	temp[0] = input / 10 + 48;
	temp[1] = input % 10 + 48;
	temp[2] = '\0';
	lcd_put_cur(0, 2);
	lcd_send_string(temp);
}

void displayHumid(uint8_t input) {
	char humid[3];
	humid[0] = input / 10 + 48;
	humid[1] = input % 10 + 48;
	humid[2] = '\0';
	lcd_put_cur(0, 8);
	lcd_send_string(humid);
}
void displayMode(uint8_t input) {
	char mode[2];
	mode[0] = input + 48;
	mode[1] = '\0';
	lcd_put_cur(0,15);
	lcd_send_string(mode);
}

void displayTimeout(void) {
	lcd_clear();
	lcd_put_cur(0, 5);
	lcd_send_string("ERROR");
	lcd_put_cur(1, 4);
	lcd_send_string("TIME_OUT");
}

void checkInformation(void) {
	char uart_buf[50];
	int uart_buf_len;
	uart_buf_len = sprintf(uart_buf, "\033[2J%s%u%s%u%s%u   \r", "T: ", maxTemp, " | H: ", maxHumid, " | P: ", period);
	HAL_UART_Transmit(&huart2, (uint8_t*) uart_buf, uart_buf_len, 100);
}

void heator(void) {
	if ((turnOnFan2(TIME_OUT_DEVICE) == HAL_TIMEOUT)
		|| (turnOnHeator(TIME_OUT_DEVICE) == HAL_TIMEOUT)
		|| (turnOffFan3(TIME_OUT_DEVICE) == HAL_TIMEOUT)
		|| (turnOffHeatpump(TIME_OUT_DEVICE) == HAL_TIMEOUT)) {
		globalTimeout = HAL_TIMEOUT;
	}
}

void heatpump(void) {
	if ((turnOffFan2(TIME_OUT_DEVICE) == HAL_TIMEOUT)
		|| (turnOffHeator(TIME_OUT_DEVICE) == HAL_TIMEOUT)
		|| (turnOnFan3(TIME_OUT_DEVICE) == HAL_TIMEOUT)
		|| (turnOnHeatpump(TIME_OUT_DEVICE) == HAL_TIMEOUT)) {
		globalTimeout = HAL_TIMEOUT;
	}
}

void normalRun(void) {
	dispatchTask();
	if (globalTimeout == HAL_TIMEOUT) return;

	if (currentMode == simulation) {
		result.temperature[0] = s_temp;
		result.humidity[0] = s_humid;
	}

	if (result.humidity[0] >= maxHumid) {
		if (!getPowerFan1()) {
			globalTimeout = turnOnFan1(TIME_OUT_DEVICE);
			if (globalTimeout == HAL_TIMEOUT) return;
		}
	} else {
		globalTimeout = turnOffFan1(TIME_OUT_DEVICE);
		if (globalTimeout == HAL_TIMEOUT) return;
	}

	if (result.temperature[0] >= maxTemp) {
		if ((turnOffFan2(TIME_OUT_DEVICE) == HAL_TIMEOUT) || (turnOffHeator(TIME_OUT_DEVICE) == HAL_TIMEOUT)) {
			globalTimeout = HAL_TIMEOUT;
			return;
		}
	}

	currentMode = buttonChangeMode(currentMode, 5, changeMode);
}

void changeMode(uint8_t inputMode) {
	switch(inputMode) {
	case normal:
		adjustedTemp = maxTemp;
		displayMode(adjustTemp);
		break;
	case adjustTemp:
		adjustedHumid = maxHumid;
		maxTemp = adjustedTemp;
		displayMode(adjustHumid);
		break;
	case adjustHumid:
		maxHumid = adjustedHumid;
		adjustedPeriod = period;
		displayMode(adjustPeriod);
		break;
	case adjustPeriod:
		period = adjustedPeriod;
		deleteAllTask();
		insertTaskBack(getTempAndHumid, 200);
		insertTaskFirstTime(heator, 0, period*ONE_SECOND);
		insertTaskFirstTime(heatpump, period*ONE_SECOND, period*ONE_SECOND);
		s_temp = s_tempAdjusted = result.temperature[0];
		s_humid = s_humidAdjusted = result.humidity[0];
		displayMode(simulation);
		HAL_GPIO_WritePin(TempOrHumidLed_Port, TempOrHumidLed_Pin, GPIO_PIN_SET);
		break;
	case simulation:
		displayMode(normal);
		HAL_GPIO_WritePin(TempOrHumidLed_Port, TempOrHumidLed_Pin, GPIO_PIN_RESET);
		break;
	}
	//checkInformation();
}

void changeChoice(uint8_t inputChoice) {
	HAL_GPIO_TogglePin(TempOrHumidLed_Port, TempOrHumidLed_Pin);
//	switch(inputChoice) {
//	case temp:
//		s_temp = s_tempAdjusted;
//		break;
//	case humid:
//		s_humid = s_humidAdjusted;
//		break;
//	}
}

void decideTempOrHumid(void) {
	switch(currentChoice) {
	case temp:
		s_temp = buttonIncreaseValue(s_temp, displayTemp, 61);
		s_temp = buttonDecreaseValue(s_temp, displayTemp, 61);
		break;
	case  humid:
		s_humid = buttonIncreaseValue(s_humid, displayHumid, 101);
		s_humid = buttonDecreaseValue(s_humid, displayHumid, 101);
		break;
	}
}

void mainFSM(void) {
	switch(currentMode) {
	case normal:
		normalRun();
		break;
	case adjustTemp:
		adjustedTemp = buttonIncreaseValue(adjustedTemp, displayChangedMaxTemp, 61);
		adjustedTemp = buttonDecreaseValue(adjustedTemp, displayChangedMaxTemp, 61);
		normalRun();
		break;
	case adjustHumid:
		adjustedHumid = buttonIncreaseValue(adjustedHumid, displayChangedMaxHum, 101);
		adjustedHumid = buttonDecreaseValue(adjustedHumid, displayChangedMaxHum, 101);
		normalRun();
		break;
	case adjustPeriod:
		adjustedPeriod = buttonIncreaseValue(adjustedPeriod, displayChangedPeriod, 61);
		adjustedPeriod = buttonDecreaseValue(adjustedPeriod, displayChangedPeriod, 61);
		normalRun();
		break;
	case simulation:
		decideTempOrHumid();
		currentChoice = buttonDecide(currentChoice, changeChoice);
		normalRun();
		break;
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
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  schedulerInit();
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start_IT(&htim3);
  //HAL_UART_Receive_IT(&huart2, data, sizeof(data));

  insertTaskFirstTime(getTempAndHumid, 120, 200);
  insertTaskFirstTime(heator, 125, period*ONE_SECOND);
  insertTaskFirstTime(heatpump, 125+period*ONE_SECOND, period*ONE_SECOND);

  lcd_init();
  lcd_put_cur(0, 5);
  lcd_send_string("LAB 5");
  HAL_Delay(1000);
  prepareLCD();
  displayMode(currentMode);
  displayChangedMaxTemp(maxTemp);
  displayChangedMaxHum(maxHumid);
  displayChangedPeriod(period);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  mainFSM();
	  if (globalTimeout == HAL_TIMEOUT) {
		  displayTimeout();
		  deleteAllTask();
		  break;
	  }
	  //globalTimeout = HAL_OK;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  htim1.Init.Prescaler = 64-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
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
  htim3.Init.Prescaler = 6400-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100-1;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA11 PA12 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC5 PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB11 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//	if (huart->Instance == huart2.Instance) {
//		HAL_UART_Receive_IT(&huart2, data, sizeof(data));
//	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if ((htim == &htim3)) {
		readIncreaseButton();
		readDecreaseButton();
		readModeButton();
		readDecideButton();
		if (getPowerFan1()) runFan1(result.humidity[0]);
		updateTask();
	}
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
