#include <inttypes.h>
#include "usart.h"
#include "TSL2561.h"
#include "i2c.h"

uint8_t TSL2561_timing_integ_value = TSL2561_TIMING_INTEG_402ms;
uint8_t TSL2561_gain_value = TSL2561_TIMING_GAIN_1X;

uint8_t TSL2561_init(void)
{
	uint8_t rev_id = 0;
	HAL_I2C_Mem_Read(&hi2c1, I2C_TSL2561_ADRES, TSL2561_CMD_BIT | 0x0A, 1, &rev_id, 1, HAL_MAX_DELAY);

	usart_write_non_DMA("TSL2561 WHO_AM_I: ");
	if(rev_id != 80)
	{
		usart_write_non_DMA("ERR");
		return 1;
	}
	else
	{
		usart_write_non_DMA("OK");
	}

	usart_crlf();


	uint8_t ctrl = TSL2561_CONTROL_POWER_ON;
	HAL_I2C_Mem_Write(&hi2c1, I2C_TSL2561_ADRES, TSL2561_CMD_BIT | 0x00, 1, &ctrl, 1, HAL_MAX_DELAY);

	uint8_t gain_timing = TSL2561_gain_value | TSL2561_timing_integ_value;
	HAL_I2C_Mem_Write(&hi2c1, I2C_TSL2561_ADRES, TSL2561_CMD_BIT | 0x01, 1, &gain_timing, 1, HAL_MAX_DELAY);

	uint8_t interrupt_level = TSL2561_INTERRUPT_INTR_LEVEL;
	HAL_I2C_Mem_Write(&hi2c1, I2C_TSL2561_ADRES, TSL2561_CMD_BIT | 0x06, 1, &interrupt_level, 1, HAL_MAX_DELAY);

	return 0;
}

unsigned int TSL2561_calculate_lux(unsigned int ch0, unsigned int ch1)
{
	unsigned long chScale;
	unsigned long channel0;
	unsigned long channel1;

	switch(TSL2561_timing_integ_value)	//scale if integration time is not 402ms
	{
		case 0:		//13.7 ms
		{
			chScale = TSL2561_CHSCALE_TINT0;
			break;
		}
		case 1:		//101 ms
		{
			chScale = TSL2561_CHSCALE_TINT1;
			break;
		}
		default:	//no scaling
		{
			chScale = (1 << TSL2561_CH_SCALE);
			break;
		}
	}

	if(TSL2561_gain_value == 0)
	{
		chScale <<= 4;
	}


	channel0 = (ch0 * chScale) >> TSL2561_CH_SCALE;
	channel1 = (ch1 * chScale) >> TSL2561_CH_SCALE;

	unsigned long ratio1 = 0;
	if (channel0 != 0) ratio1 = (channel1 << (TSL2561_RATIO_SCALE +1)) / channel0;

	unsigned long ratio = (ratio1 + 1) >> 1;

	unsigned int b,m;

	if ((ratio >= 0) && (ratio <= TSL2561_K1T))
		{b=TSL2561_B1T; m=TSL2561_M1T;}
	else if (ratio <= TSL2561_K2T)
		{b=TSL2561_B2T; m=TSL2561_M2T;}
	else if (ratio <= TSL2561_K3T)
		{b=TSL2561_B3T; m=TSL2561_M3T;}
	else if (ratio <= TSL2561_K4T)
		{b=TSL2561_B4T; m=TSL2561_M4T;}
	else if (ratio <= TSL2561_K5T)
		{b=TSL2561_B5T; m=TSL2561_M5T;}
	else if (ratio <= TSL2561_K6T)
		{b=TSL2561_B6T; m=TSL2561_M6T;}
	else if (ratio <= TSL2561_K7T)
		{b=TSL2561_B7T; m=TSL2561_M7T;}
	else if (ratio > TSL2561_K8T)
		{b=TSL2561_B8T; m=TSL2561_M8T;}

	unsigned long temp;

	temp = ((channel0 * b) - (channel1 * m));

	if(temp < 0)
	{
		temp = 0;
	}

	temp += (1 << (TSL2561_LUX_SCALE - 1));

	unsigned long lux = temp >> TSL2561_LUX_SCALE;

	return lux;
}

unsigned int TSL2561_read(void)
{
	uint8_t tab[4];
	uint16_t DATA0, DATA1;

	HAL_I2C_Mem_Read(&hi2c1, I2C_TSL2561_ADRES, TSL2561_CMD_BIT | 0x0C, 1, tab, 4, HAL_MAX_DELAY);
	DATA0 = (tab[1] << 8) | tab[0];
	DATA1 = (tab[3] << 8) | tab[2];

	return TSL2561_calculate_lux(DATA0, DATA1);
}

void TSL2561_print_value(void)
{
	uint32_t lux;
	lux = TSL2561_read();

	usart_uint16(lux, 10);
	usart_write_non_DMA(" lux");
}
