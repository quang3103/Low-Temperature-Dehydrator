/*
 * fan.h
 *
 *  Created on: Dec 23, 2020
 *      Author: MSI-Modern
 */

#define Fan1_Port GPIOC
#define Fan1_Pin GPIO_PIN_6

#define Fan2_Port GPIOC
#define Fan2_Pin GPIO_PIN_5

#define Fan3_Port GPIOA
#define Fan3_Pin GPIO_PIN_12

HAL_StatusTypeDef turnOnFan1(uint8_t Timeout);
HAL_StatusTypeDef turnOffFan1(uint8_t Timeout);

HAL_StatusTypeDef turnOnFan2(uint8_t Timeout);
HAL_StatusTypeDef turnOffFan2(uint8_t Timeout);

HAL_StatusTypeDef turnOnFan3(uint8_t Timeout);
HAL_StatusTypeDef turnOffFan3(uint8_t Timeout);

void runFan1(uint8_t humidity);
uint8_t getPowerFan1(void);
