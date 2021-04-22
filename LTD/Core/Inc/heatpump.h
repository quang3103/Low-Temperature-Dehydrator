/*
 * heatpump.h
 *
 *  Created on: Dec 23, 2020
 *      Author: MSI-Modern
 */

#define Heatpump_Port GPIOB
#define Heatpump_Pin GPIO_PIN_12

HAL_StatusTypeDef turnOnHeatpump(uint8_t Timeout);
HAL_StatusTypeDef turnOffHeatpump(uint8_t Timeout);
