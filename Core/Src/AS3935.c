#include <inttypes.h>
#include "i2c.h"
#include "AS3935.h"
#include "usart.h"
#include "my_usart.h"

uint8_t AS3935_interrupt_flag;

void AS3935_init(void)
{
	usart_write_non_DMA("AS3935\n");

	uint8_t power_up = 0;
	HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_AFE_GAIN_BOOST_POWER_REG, 1, &power_up, 1, HAL_MAX_DELAY);			//power-up

	uint8_t calib_value = 0x96;
	HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_CALIB_RCO_REG, 1, &calib_value, 1, HAL_MAX_DELAY);		//calib_rco

	uint8_t disp_value = 0x40;
	HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_DISP_REG, 1, &disp_value, 1, HAL_MAX_DELAY);		//enable SRCO on IRQ pin
	HAL_Delay(2);
	disp_value = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_DISP_REG, 1, &disp_value, 1, HAL_MAX_DELAY);		//disable SRCO on IRQ pin

	uint8_t TRCO_CALIB;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_TRCO_CALIB_REG, 1, &TRCO_CALIB, 1, HAL_MAX_DELAY);	//check if TRCO successfull
	TRCO_CALIB &= 0x40;

	uint8_t SRCO_CALIB;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_SRCO_CALIB_REG, 1, &SRCO_CALIB, 1, HAL_MAX_DELAY);	//check if SRCO successfull
	SRCO_CALIB &= 0x40;

	if(TRCO_CALIB != 0 || SRCO_CALIB != 0)
	{
		usart_write_non_DMA("calib err");
	}

	AS3935_set_spike_rejection(0);
	AS3935_set_AFE_gain_boost(AS3935_AFE_INDOOR);
	AS3935_set_minimum_number_lightning(AS3935_MIN_LIGHTNING_1);
	AS3935_set_watchdog_threshold(2);
	AS3935_set_noise_floor_level(0);
	AS3935_set_mask_disturber(0);

	AS3935_print_parameters();
}

void AS3935_set_spike_rejection(uint8_t v)
{
	if(v < 12)
	{
		uint8_t temp = 0;
		HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_LIGHTNING_SPIKE_REJECTION_REG, 1, &temp, 1, HAL_MAX_DELAY);

		temp = (temp & 0xF0) | v;

		HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_LIGHTNING_SPIKE_REJECTION_REG, 1, &temp, 1, HAL_MAX_DELAY);
	}
}

void AS3935_set_minimum_number_lightning(uint8_t v)
{
	if(v < 4)
	{
		v <<= 4;
		uint8_t temp = 0;
		HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_LIGHTNING_SPIKE_REJECTION_REG, 1, &temp, 1, HAL_MAX_DELAY);

		temp = (temp & 0xCF) | v;

		HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_LIGHTNING_SPIKE_REJECTION_REG, 1, &temp, 1, HAL_MAX_DELAY);
	}
}

void AS3935_set_AFE_gain_boost(uint8_t v)
{
	if(v == AS3935_AFE_INDOOR || v == AS3935_AFE_OUTDOOR)
	{
		v <<= 1;
		uint8_t temp = 0;
		HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_AFE_GAIN_BOOST_POWER_REG, 1, &temp, 1, HAL_MAX_DELAY);
		temp = (temp & 0xC1) | v;
		HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_AFE_GAIN_BOOST_POWER_REG, 1, &temp, 1, HAL_MAX_DELAY);
	}
}

void AS3935_set_noise_floor_level(uint8_t v)
{
	if(v < 8)
	{
		v <<= 4;

		uint8_t temp = 0;
		HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_NOISE_FLOOR_LEVEL_WATCHDOG_THRESHOLD_REG, 1, &temp, 1, HAL_MAX_DELAY);
		temp = (temp & 0x8F) | v;
		HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_NOISE_FLOOR_LEVEL_WATCHDOG_THRESHOLD_REG, 1, &temp, 1, HAL_MAX_DELAY);
	}
}

void AS3935_set_watchdog_threshold(uint8_t v)
{
	if(v < 11)
	{
		uint8_t temp = 0;
		HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_NOISE_FLOOR_LEVEL_WATCHDOG_THRESHOLD_REG, 1, &temp, 1, HAL_MAX_DELAY);

		temp = (temp & 0xF0) | v;

		HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_NOISE_FLOOR_LEVEL_WATCHDOG_THRESHOLD_REG, 1, &temp, 1, HAL_MAX_DELAY);
	}
}

void AS3935_set_mask_disturber(uint8_t v)
{
	if(v < 2)
	{
		v <<= 5;

		uint8_t temp = 0;
		HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_LCO_FDIV_MASK_DIST_INTERUPT_REG, 1, &temp, 1, HAL_MAX_DELAY);

		temp = (temp & 0xDF) | v;

		HAL_I2C_Mem_Write(&hi2c1, AS3935_ADDRESS, AS3935_LCO_FDIV_MASK_DIST_INTERUPT_REG, 1, &temp, 1, HAL_MAX_DELAY);
	}
}

void AS3935_get_AFE_gain_boost(void)
{
	usart_write_non_DMA("AFE: ");
	uint8_t afe_gain = 0;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_AFE_GAIN_BOOST_POWER_REG, 1, &afe_gain, 1, HAL_MAX_DELAY);
	afe_gain = (afe_gain & 0x3E ) >> 1;
	if(afe_gain == AS3935_AFE_OUTDOOR)
	{
		usart_write_non_DMA("Outdoor");
	}
	else if (afe_gain == AS3935_AFE_INDOOR)
	{
		usart_write_non_DMA("Indoor");
	}
	usart_crlf();
}

void AS3935_get_noise_floor_level(void)
{
	usart_write_non_DMA("Noise floor level (Outdoor/Indoor): ");
	uint8_t nf_lev = 0;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_NOISE_FLOOR_LEVEL_WATCHDOG_THRESHOLD_REG, 1, &nf_lev, 1, HAL_MAX_DELAY);
	nf_lev = (nf_lev & 0x70) >> 4;
	usart_uint(nf_lev, 10);

	usart_write_non_DMA(" ");
	switch(nf_lev)
	{
		case 0:
		{
			usart_write_non_DMA("390/28");
			break;
		}
		case 1:
		{
			usart_write_non_DMA("630/45");
			break;
		}
		case 2:
		{
			usart_write_non_DMA("860/62");
			break;
		}
		case 3:
		{
			usart_write_non_DMA("1100/78");
			break;
		}
		case 4:
		{
			usart_write_non_DMA("1140/95");
			break;
		}
		case 5:
		{
			usart_write_non_DMA("1570/112");
			break;
		}
		case 6:
		{
			usart_write_non_DMA("1800/130");
			break;
		}
		case 7:
		{
			usart_write_non_DMA("2000/146");
			break;
		}
	}

	usart_crlf();
}

void AS3935_get_watchdog_threshold(void)
{
	usart_write_non_DMA("Watchdog Threshold: ");
	uint8_t wdth = 0;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_NOISE_FLOOR_LEVEL_WATCHDOG_THRESHOLD_REG, 1, &wdth, 1, HAL_MAX_DELAY);
	wdth = wdth & 0x0F;
	usart_uint(wdth, 10);

	usart_crlf();
}

void AS3935_get_number_of_lightning(void)
{
	usart_write_non_DMA("Minimum number of lightning: ");
	uint8_t number_of_lightning;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_LIGHTNING_SPIKE_REJECTION_REG, 1, &number_of_lightning, 1, HAL_MAX_DELAY);

	number_of_lightning = (number_of_lightning & 0x30) >> 4;

	uint8_t lightning_arr[4] = {1,5,9,16};
	usart_uint(lightning_arr[number_of_lightning], 10);

	usart_crlf();
}

void AS3935_get_spike_rejection(void)
{
	usart_write_non_DMA("Spike rejection: ");
	uint8_t srej = 0;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_LIGHTNING_SPIKE_REJECTION_REG, 1, &srej, 1, HAL_MAX_DELAY);
	srej = srej & 0x0F;
	usart_uint(srej, 10);
	usart_crlf();
}

void AS3935_get_mask_disturber(void)
{
	usart_write_non_DMA("Mask disturber: ");
	uint8_t mask_dist = 0;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_LCO_FDIV_MASK_DIST_INTERUPT_REG, 1, &mask_dist, 1, HAL_MAX_DELAY);
	mask_dist = mask_dist & 0x20;
	if(mask_dist)
	{
		usart_write_non_DMA("enabled");
	}
	else
	{
		usart_write_non_DMA("disabled");
	}
	usart_crlf();
}

void AS3935_print_parameters(void)
{
	AS3935_get_AFE_gain_boost();

	AS3935_get_number_of_lightning();

	AS3935_get_spike_rejection();

	AS3935_get_noise_floor_level();

	AS3935_get_watchdog_threshold();

	AS3935_get_mask_disturber();
}

uint32_t AS3935_get_lightning_energy(void)	/*get energy of single lightning*/
{
	uint32_t energy = 0;
	uint8_t buffer[3] = {0};

	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_ENERGY_LIGHTNING_LSB_REG, 1, buffer, 3, HAL_MAX_DELAY);

	buffer[2] &= 0x1F; 	/* MMSBYTE*/

	energy = (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]);
	return energy;
}

uint8_t AS3935_get_distance(void)	/*get estimation distance in meters of single lightning*/
{
	uint8_t v = 0;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_DISTANCE_REG, 1, &v, 1, HAL_MAX_DELAY);
	v &= 0x3F;
	return v;
}

void AS3935_get_interrupt_reason(void)	/*get info what caused interrupt */
{
	//After the signal IRQ goes high the external unit should wait 2ms before reading the interrupt register. The
	//interrupt bus IRQ is set back to low whenever the interrupt register is read out.
	uint8_t v = 0;
	HAL_I2C_Mem_Read(&hi2c1, AS3935_ADDRESS, AS3935_LCO_FDIV_MASK_DIST_INTERUPT_REG, 1, &v, 1, HAL_MAX_DELAY);
	v &= 0x0F;

	if(v == 0x01)
	{
		usart_write_non_DMA("Noise level too high");
	}
	else if(v == 0x04)
	{
		usart_write_non_DMA("Disturber detected");
	}
	else if(v == 0x08)
	{
		usart_write_non_DMA("Lightning interrupt");

		uint8_t distance = AS3935_get_distance();
		usart_crlf();
		usart_write_non_DMA("Distance: ");
		usart_int(distance, 10);
		usart_write_non_DMA(" km away");
		usart_crlf();

		uint32_t lightning_energy = AS3935_get_lightning_energy();
		usart_write_non_DMA("Energy: ");
		usart_uint32(lightning_energy, 10);
	}
	else
	{
		usart_write_non_DMA("Interrupt description unknown value: ");
		usart_int(v,10);
	}
}

uint8_t AS3935_get_lightning_flag(void)
{
	return AS3935_interrupt_flag;
}

void AS3935_set_lightning_flag(void)
{
	AS3935_interrupt_flag = 1;
}

void AS3935_clear_lightning_flag(void)
{
	AS3935_interrupt_flag = 0;
}
