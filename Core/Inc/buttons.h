/*
 * buttons.h
 *
 *  Created on: 17 maj 2023
 *      Author: Owiec
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "gpio.h"

enum {RELEASED = 0, PRESSED};

typedef struct
{
	GPIO_TypeDef* gpio_port;
	uint16_t gpio_pin;
	uint16_t cnt;
	uint16_t tick_value;
	uint8_t increment_value;
	uint8_t status;
	GPIO_PinState pressed_state;
	void (*PressedCallback)(void);
}button_t;

extern button_t btn1, btn2;
void ButtonEvent(button_t *btn);
void ButtonPressedCallbackRegister(button_t *btn, void (*PressedCallbackRegisterPtr)(void));
void ButtonInit(button_t *btn, GPIO_TypeDef* port, uint16_t pin, GPIO_PinState pressedstate, uint16_t increment_value, uint16_t tick_value);

#endif /* INC_BUTTONS_H_ */
