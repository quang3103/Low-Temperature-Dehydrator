/*
 * heator.h
 *
 *  Created on: Dec 23, 2020
 *      Author: MSI-Modern
 */

#define Heator_Port GPIOA
#define Heator_Pin GPIO_PIN_11

void setMaxTemperature(uint8_t temperature);
uint8_t getMaxTemperature(void);

HAL_StatusTypeDef turnOnHeator(uint8_t Timeout);
HAL_StatusTypeDef turnOffHeator(uint8_t Timeout);

