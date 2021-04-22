/*
 * button.c
 *
 *  Created on: Dec 22, 2020
 *      Author: MSI-Modern
 */

#include "stm32f1xx_hal.h"
#include "button.h"

GPIO_PinState firstReadInc, secondReadInc;
GPIO_PinState firstReadDec, secondReadDec;
GPIO_PinState firstReadMode, secondReadMode;
GPIO_PinState R1,R2;

Button_Status increaseButtonStatus = 0, decreaseButtonStatus = 0, modeButtonStatus = 0, decideButtonStatus = 0;
Button_Status isIncBtnPressed = 0, isDecBtnPressed = 0, isModeBtnPressed = 0, isDecideBtnPressed = 0;

uint16_t timeInc = 0, timeDec = 0;

void readIncreaseButton(void) {
	firstReadInc = secondReadInc;
	secondReadInc = !HAL_GPIO_ReadPin(IncBtn_Port, IncBtn_Pin);
	if (firstReadInc == secondReadInc) {
		if (firstReadInc) {
			timeInc++;
			if (timeInc >= (BUTTON_THRESHOLD + BUTTON_PERIOD)) {
				timeInc = BUTTON_THRESHOLD;
				isIncBtnPressed = Button_Pressed_Increase;
			} else {
				isIncBtnPressed = Button_Pressed;
			}
		} else {
			timeInc = 0;
			isIncBtnPressed = Button_Not_Pressed;
		}
	}
}

uint8_t buttonIncreaseValue(uint8_t inputValue, void(* function)(uint8_t), uint8_t threshold) {
	switch(increaseButtonStatus) {
	case Button_Not_Pressed:
		if (isIncBtnPressed == Button_Pressed) {
			inputValue += BUTTON_STEP;
			inputValue %= threshold;
			(* function)(inputValue);
		}
		increaseButtonStatus = isIncBtnPressed;
		break;
	case Button_Pressed:
		increaseButtonStatus = isIncBtnPressed;
		break;
	case Button_Pressed_Increase:
		inputValue += BUTTON_STEP;
		inputValue %= threshold;
		(* function)(inputValue);
		increaseButtonStatus = isIncBtnPressed = Button_Pressed;
		break;
	}
	return inputValue;
}

void readDecreaseButton(void) {
	firstReadDec = secondReadDec;
	secondReadDec = !HAL_GPIO_ReadPin(DecBtn_Port, DecBtn_Pin) && HAL_GPIO_ReadPin(IncBtn_Port, IncBtn_Pin);
	if (firstReadDec == secondReadDec) {
		if (firstReadDec) {
			timeDec++;
			if (timeDec >= (BUTTON_THRESHOLD + BUTTON_PERIOD)) {
				timeDec = BUTTON_THRESHOLD;
				isDecBtnPressed = Button_Pressed_Increase;
			} else {
				isDecBtnPressed = Button_Pressed;
			}
		} else {
			timeDec = 0;
			isDecBtnPressed = Button_Not_Pressed;
		}
	}
}


uint8_t buttonDecreaseValue(uint8_t inputValue, void(* function)(uint8_t), uint8_t threshold) {
	switch(decreaseButtonStatus) {
	case Button_Not_Pressed:
		if (isDecBtnPressed == Button_Pressed) {
			inputValue -= BUTTON_STEP;
			if (inputValue > threshold) inputValue = threshold - BUTTON_STEP;
			else inputValue = inputValue % threshold;
			(* function)(inputValue);
		}
		decreaseButtonStatus = isDecBtnPressed;
		break;
	case Button_Pressed:
		decreaseButtonStatus = isDecBtnPressed;
		break;
	case Button_Pressed_Increase:
		inputValue -= BUTTON_STEP;
		if (inputValue > threshold) inputValue = threshold - BUTTON_STEP;
		else inputValue = inputValue % threshold;
		(* function)(inputValue);
		decreaseButtonStatus = isDecBtnPressed = Button_Pressed;
		break;
	}
	return inputValue;
}

void readModeButton(void) {
	firstReadMode = secondReadMode;
	secondReadMode = !HAL_GPIO_ReadPin(ModeBtn_Port, ModeBtn_Pin);
	if (firstReadMode==secondReadMode) {
		isModeBtnPressed = firstReadMode;
	}
}

uint8_t buttonChangeMode(uint8_t mode, uint8_t maxNumberOfMode, void(* function)(uint8_t)) {
	switch(modeButtonStatus) {
	case Button_Not_Pressed:
		if (isModeBtnPressed) {
			(* function)(mode);
			mode = (mode + 1) % maxNumberOfMode;
		}
		modeButtonStatus = isModeBtnPressed;
		break;
	case Button_Pressed: case Button_Pressed_Increase:
		modeButtonStatus = isModeBtnPressed;
		break;
	}
	return mode;
}

void readDecideButton(void) {
	R1 = R2;
	R2 = !HAL_GPIO_ReadPin(TempOrHumidBtn_Port, TempOrHumidBtn_Pin);
	if (R1 == R2) {
		isDecideBtnPressed = R1;
	}
}

uint8_t buttonDecide(uint8_t mode, void (* function)(uint8_t)) {
	switch(decideButtonStatus) {
	case Button_Not_Pressed:
		if (isDecideBtnPressed) {
			(* function)(mode);
			mode = (mode + 1) % 2;
		}
		decideButtonStatus = isDecideBtnPressed;
		break;
	case Button_Pressed: case Button_Pressed_Increase:
		decideButtonStatus = isDecideBtnPressed;
		break;
	}
	return mode;
}
