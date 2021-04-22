/*
 * heatpump.c
 *
 *  Created on: Dec 23, 2020
 *      Author: MSI-Modern
 */

#include "stm32f1xx_hal.h"
#include "heatpump.h"
#include <stdlib.h>

HAL_StatusTypeDef turnOnHeatpump(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Heatpump_Port, Heatpump_Pin, GPIO_PIN_SET);

	while (!HAL_GPIO_ReadPin(Heatpump_Port, Heatpump_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Heatpump_Port, Heatpump_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}

HAL_StatusTypeDef turnOffHeatpump(uint8_t Timeout) {
	uint32_t timeStart = HAL_GetTick();

	HAL_GPIO_WritePin(Heatpump_Port, Heatpump_Pin, GPIO_PIN_RESET);

	while (HAL_GPIO_ReadPin(Heatpump_Port, Heatpump_Pin)) {
		if (abs(HAL_GetTick() - timeStart) > Timeout) return HAL_TIMEOUT;
		HAL_GPIO_WritePin(Heatpump_Port, Heatpump_Pin, GPIO_PIN_RESET);
	}
	return HAL_OK;
}
