/*
 * my_usart.h
 *
 *  Created on: Nov 28, 2021
 *      Author: mikio
 */

#ifndef INC_MY_USART_H_
#define INC_MY_USART_H_

#define USART_BUFFER_LENGTH 100

extern volatile char usart_rx_buffer[USART_BUFFER_LENGTH];
extern volatile char usart_tx_buffer[USART_BUFFER_LENGTH];

void set_usart_flag(uint8_t v);

uint8_t get_usart_flag(void);

void usart_init(void);

void usart_uint(uint8_t v, uint8_t system);

void usart_int(int8_t v, uint8_t system);

void usart_int16(int16_t v, uint8_t system);

void usart_uint16(uint16_t v, uint8_t system);

void usart_uint32(uint32_t v, uint8_t system);

void usart_write(volatile char *str);

void usart_write_non_DMA(char *str);

void usart_crlf(void);

uint8_t find_command(char *ptr);

int16_t get_parameter_int(char delim);

char* get_parameter_string(char delim);

void usart_handle(void);

#endif /* INC_MY_USART_H_ */
