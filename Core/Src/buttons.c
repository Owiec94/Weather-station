/*
 * buttons.c
 *
 *  Created on: 17 maj 2023
 *      Author: Owiec
 */
#include "buttons.h"
#include "main.h"

button_t btn1, btn2;

void ButtonEvent(button_t *btn)
{
	if(HAL_GPIO_ReadPin(btn->gpio_port, btn->gpio_pin) == btn->pressed_state)
	{
		btn->cnt += btn->increment_value;

		if(btn->cnt >= btn->tick_value && btn->status == RELEASED)
		{
			btn->cnt = 0;
			btn->status = PRESSED;
			if(btn->PressedCallback != NULL)
			{
				btn->PressedCallback();
			}
		}
	}
	else
	{
		btn->cnt = 0;
		btn->status = RELEASED;
	}
}

void ButtonPressedCallbackRegister(button_t *btn, void (*PressedCallbackRegisterPtr)(void))
{
	btn->PressedCallback = PressedCallbackRegisterPtr;
}

void ButtonInit(button_t *btn, GPIO_TypeDef* port, uint16_t pin, GPIO_PinState pressedstate, uint16_t increment_value, uint16_t tick_value)
{
	btn->gpio_port = port;
	btn->gpio_pin = pin;
	btn->pressed_state = pressedstate;
	btn->increment_value = increment_value;
	btn->tick_value = tick_value;
}
