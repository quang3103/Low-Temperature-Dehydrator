/*
 * button.h
 *
 *  Created on: Dec 22, 2020
 *      Author: MSI-Modern
 */

#define BUTTON_THRESHOLD 100
#define BUTTON_PERIOD 50
#define BUTTON_STEP 1

#define IncBtn_Port GPIOC
#define IncBtn_Pin GPIO_PIN_9

#define DecBtn_Port GPIOC
#define DecBtn_Pin GPIO_PIN_8

#define ModeBtn_Port GPIOB
#define ModeBtn_Pin GPIO_PIN_8

#define TempOrHumidBtn_Port GPIOB
#define TempOrHumidBtn_Pin GPIO_PIN_11

#define TempOrHumidLed_Port GPIOB
#define TempOrHumidLed_Pin GPIO_PIN_2

typedef enum {
	Button_Not_Pressed = 0,
	Button_Pressed,
	Button_Pressed_Increase
} Button_Status;

void readDecreaseButton(void);
void readIncreaseButton(void);
uint8_t buttonIncreaseValue(uint8_t inputValue, void(* function)(uint8_t), uint8_t threshold);
uint8_t buttonDecreaseValue(uint8_t inputValue, void(* function)(uint8_t), uint8_t threshold);

void readModeButton(void);
uint8_t buttonChangeMode(uint8_t mode, uint8_t maxNumberOfMode, void(* function)(uint8_t));

void readDecideButton(void);
uint8_t buttonDecide(uint8_t mode, void (* function)(uint8_t));
