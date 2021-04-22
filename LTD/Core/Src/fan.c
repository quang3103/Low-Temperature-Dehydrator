/*
 * fan.c
 *
 *  Created on: Dec 23, 2020
 *      Author: MSI-Modern
 */

#include "stm32f1xx_hal.h"
#include "fan.h"
#include <stdlib.h>

uint8_t powerOfFan1 = 0;
uint8_t countFan = 0;

HAL_StatusTypeDef turnOnFan1(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	powerOfFan1 = 1;
	countFan = 0;
	HAL_GPIO_WritePin(Fan1_Port, Fan1_Pin, GPIO_PIN_SET);

	while (!HAL_GPIO_ReadPin(Fan1_Port, Fan1_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Fan1_Port, Fan1_Pin, GPIO_PIN_SET);
	}
	return HAL_OK;
}

uint8_t getPowerFan1(void) {
	return powerOfFan1;
}

void runFan1(uint8_t humidity) {
	uint8_t frequency = 100 - (humidity/10)*10;
	countFan++;
	if (countFan == frequency) {
		countFan = 0;
		HAL_GPIO_TogglePin(Fan1_Port, Fan1_Pin);
	}
}

HAL_StatusTypeDef turnOffFan1(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	powerOfFan1 = 0;
	HAL_GPIO_WritePin(Fan1_Port, Fan1_Pin, GPIO_PIN_RESET);

	while (HAL_GPIO_ReadPin(Fan1_Port, Fan1_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Fan1_Port, Fan1_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}

HAL_StatusTypeDef turnOnFan2(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Fan2_Port, Fan2_Pin, GPIO_PIN_SET);

	while (!HAL_GPIO_ReadPin(Fan2_Port, Fan2_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Fan2_Port, Fan2_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}

HAL_StatusTypeDef turnOffFan2(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Fan2_Port, Fan2_Pin, GPIO_PIN_RESET);

	while (HAL_GPIO_ReadPin(Fan2_Port, Fan2_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Fan2_Port, Fan2_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}

HAL_StatusTypeDef turnOnFan3(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Fan3_Port, Fan3_Pin, GPIO_PIN_SET);

	while (!HAL_GPIO_ReadPin(Fan3_Port, Fan3_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Fan3_Port, Fan3_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}

HAL_StatusTypeDef turnOffFan3(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Fan3_Port, Fan3_Pin, GPIO_PIN_RESET);

	while (HAL_GPIO_ReadPin(Fan3_Port, Fan3_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Fan3_Port, Fan3_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}
