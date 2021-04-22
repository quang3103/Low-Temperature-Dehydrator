/*
 * heator.c
 *
 *  Created on: Dec 23, 2020
 *      Author: MSI-Modern
 */

#include "stm32f1xx_hal.h"
#include "heator.h"
#include <stdlib.h>

uint8_t maxTemperature = 50;

void setMaxTemperature(uint8_t temperature) {
	maxTemperature = temperature;
}

uint8_t getMaxTemperature(void) {
	return maxTemperature;
}

HAL_StatusTypeDef turnOnHeator(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Heator_Port, Heator_Pin, GPIO_PIN_SET);

	while (!HAL_GPIO_ReadPin(Heator_Port, Heator_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Heator_Port, Heator_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}

HAL_StatusTypeDef turnOffHeator(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Heator_Port, Heator_Pin, GPIO_PIN_RESET);

	while (HAL_GPIO_ReadPin(Heator_Port, Heator_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Heator_Port, Heator_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}
