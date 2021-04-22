/*
 * DHT11.c
 *
 *  Created on: Dec 24, 2020
 *      Author: MSI-Modern
 */

#include "stm32f1xx_hal.h"
#include "DHT11.h"
#include <stdlib.h>

void delay(uint16_t inputDelay) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER(&htim1) < inputDelay);
}

void setPinOutput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void setPinInput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

uint8_t startDHT11(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	uint8_t reponse = 0;
	setPinOutput(GPIOA, GPIO_PIN_1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_Delay(18);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	delay(30);
	setPinInput(GPIOA, GPIO_PIN_1);
	//while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1));
	delay(40);
	if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) {
		delay(80);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) {
			reponse = 1;
		}
	}
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return 0;
	}
	return reponse;
}

uint8_t readDHT11(void) {
	uint8_t i,ans = 0;
	for (i = 0; i < 8; i++) {
		ans = ans << 1;
		while (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1))); //wait until the pin high
		delay(40);
		ans = ans | HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
		while (HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_1));  // wait for the pin to go low
	}
	return ans;
}

HAL_StatusTypeDef getDHT11Data(DHT11_data *data, uint16_t Timeout) {
	uint8_t sum;
	uint8_t humidity[2] = {0,0};
	uint8_t temperature[2] = {0,0};

	if (startDHT11(Timeout)) {
		humidity[0] = readDHT11();
		humidity[1] = readDHT11();
		temperature[0] = readDHT11();
		temperature[1] = readDHT11();
		sum = readDHT11();
		if (sum == humidity[0]+humidity[1]+temperature[0]+temperature[1]) {
			data->humidity[0] = humidity[0];
			data->humidity[1] = humidity[1];
			data->temperature[0] = temperature[0];
			data->temperature[1] = temperature[1];
			return HAL_OK;
		}
	} else {
		return HAL_TIMEOUT;
	}
}
