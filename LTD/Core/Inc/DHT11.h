/*
 * DHT11.h
 *
 *  Created on: Dec 24, 2020
 *      Author: MSI-Modern
 */

#define DHT11_Port GPIOA
#define DHT11_Pin GPIO_PIN_1

typedef struct data {
	uint8_t humidity[2];
	uint8_t temperature[2];
} DHT11_data;

TIM_HandleTypeDef htim1;

void delay(uint16_t);

void setPinOutput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void setPinInput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

uint8_t startDHT11(uint8_t Timeout);
uint8_t readDHT11(void);

HAL_StatusTypeDef getDHT11Data(DHT11_data *data, uint16_t Timeout);

//static void MX_TIM1_Init(void);
