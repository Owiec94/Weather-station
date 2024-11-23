/*
 * my_usart.c
 *
 *  Created on: Nov 28, 2021
 *      Author: mikio
 */
#include <inttypes.h>
#include "usart.h"
#include "my_usart.h"

volatile char usart_rx_buffer[USART_BUFFER_LENGTH] = {0};
volatile char usart_tx_buffer[USART_BUFFER_LENGTH] = {0};

volatile uint8_t usart_flag = 0;

void set_usart_flag(uint8_t v)
{
	usart_flag = v;
}

volatile uint8_t get_usart_flag(void)
{
	return usart_flag;
}

void usart_init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, usart_rx_buffer, USART_BUFFER_LENGTH);
}

void usart_uint(uint8_t v, uint8_t system)
{
	char bufor[9];
	itoa(v,bufor,system);
	usart_write_non_DMA(bufor);
}

void usart_int(int8_t v, uint8_t system)
{
	char bufor[9];
	itoa(v,bufor,system);
	usart_write_non_DMA(bufor);
}

void usart_int16(int16_t v, uint8_t system)
{
	char bufor[17];
	itoa(v,bufor,system);
	usart_write_non_DMA(bufor);
}

void usart_uint16(uint16_t v, uint8_t system)
{
	char bufor[17];
	itoa(v,bufor,system);
	usart_write_non_DMA(bufor);
}

void usart_uint32(uint32_t v, uint8_t system)
{
	char bufor[33];
	itoa(v, bufor, system);
	usart_write_non_DMA(bufor);
}

void usart_write(volatile char *str)
{
	HAL_UART_Transmit_DMA(&huart2, str, strlen(str));
}

void usart_write_non_DMA(char *str)
{
	HAL_UART_Transmit(&huart2, str, strlen(str), HAL_MAX_DELAY);
}

void usart_crlf(void)
{
	usart_write_non_DMA("\r\n");
}

uint8_t find_command(char *ptr)
{
	return strncmp(usart_rx_buffer, ptr, strlen(ptr));
}

int16_t get_parameter_int(char delim)
{
	int16_t param = 0;
	char *ptr = strchr(usart_rx_buffer, delim);
	if(ptr != NULL)
	{
		param = atoi(ptr+1);
	}
	return param;
}

char* get_parameter_string(char delim)
{
	char *param = "";
	char *ptr = strchr(usart_rx_buffer, delim);
	if(ptr != NULL)
	{
		param = (ptr+1);
	}
	return param;
}

void usart_handle(void)
{
	/*if(true == get_usart_flag())
	{
		set_usart_flag(false);

		bool found_command = false;

		for(uint8_t i = 0; i < COMMAND_MAPPER_SIZE; i++)
		{
		  if(0 == find_command(command_mapper[i].usart_cmd))
		  {
			command_mapper[i].trigger();
			found_command = true;
			break;
		  }
		}

		if(false == found_command)
		{
		    debugln("error, unknown command");
		}

		memset(usart_rx_buffer, 0, USART_BUFFER_LENGTH);
	}*/
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART2)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, usart_rx_buffer, USART_BUFFER_LENGTH);
		//set_usart_flag(true);
	}

}



