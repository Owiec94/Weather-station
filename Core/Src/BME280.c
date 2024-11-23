#include "BME280.h"
#include "i2c.h"
#include "my_usart.h"

uint8_t BME280_check_connection(void)
{
	uint8_t who_am_i = 0;
	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, BME280_WHO_AM_I_REG, 1, &who_am_i, 1, HAL_MAX_DELAY);
	return who_am_i;
}

uint8_t BME280_init(BME_sensor *BME280)
{
	usart_write_non_DMA("BME280 WHO_AM_I: ");
	if(BME280_check_connection() != BME280_WHO_AM_I_VAL)
	{
		usart_write_non_DMA("ERR");
		return 1;
	}
	else
	{
		usart_write_non_DMA("OK");
	}

	BME280->temp_calib = -1;
	BME280->pressure_calib = 0;
	BME280->humidity_calib = 4;

	BME280_load_temperature_calibration(BME280);
	BME280_load_pressure_calibration(BME280);
	BME280_load_humidity_calibration(BME280);

	uint8_t ctrl_hum_reg_val = OVERSAMPLING_HUMIDITY_X4;
	HAL_I2C_Mem_Write(&hi2c1, I2C_BME_ADDRESS, BME280_CTRL_HUM_REG, 1, &ctrl_hum_reg_val, 1, HAL_MAX_DELAY);

	uint8_t config_reg_val = TIME_STANDBY_10 | FILTER_2 | DISABLE_SPI3W;
	HAL_I2C_Mem_Write(&hi2c1, I2C_BME_ADDRESS, BME_280_CONFIG_REG, 1, &config_reg_val , 1, HAL_MAX_DELAY);

	uint8_t ctrl_meas_reg_val = OVERSAMPLING_TEMPERATURE_X1 | OVERSAMPLING_PRESSURE_X16 | MODE_NORMAL_MODE;
	HAL_I2C_Mem_Write(&hi2c1, I2C_BME_ADDRESS, BME280_CTRL_MEAS_REG, 1, &ctrl_meas_reg_val , 1, HAL_MAX_DELAY);

	return 0;
}

void BME280_read_temperature_RAW(int32_t *temp)
{
	uint8_t buffer[3];
	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, 0XFA, 1, buffer, 3, HAL_MAX_DELAY);//3 rejestry po 6 bitow
	(*temp) =  (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
}

void BME280_read_pressure_RAW(int32_t* pressure)
{
	uint8_t buffer[3];
	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, 0XF7, 1, buffer, 3, HAL_MAX_DELAY);
	(*pressure) =  (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
}

void BME280_read_humidity_RAW(int32_t *humidity)
{
	uint8_t buffer[2];
	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, 0XFD, 1, buffer, 2, HAL_MAX_DELAY);
	(*humidity) = (buffer[0] << 8) | buffer[1];
}

void BME280_calc_temperature(BME_sensor *BME)
{
	int32_t var1, var2;
	int32_t temperature;

	var1 = 	((((BME->temperature_RAW >> 3) - ((int32_t)BME->calib_T1 << 1))) *
			((int32_t)BME->calib_T2)) >> 11;
	var2 =  (((((BME->temperature_RAW >> 4) - ((int32_t)BME->calib_T1)) *
			((BME->temperature_RAW >> 4) - ((int32_t)BME->calib_T1))) >> 12) *
			((int32_t)BME->calib_T3)) >> 14;
	BME->t_fine = var1 + var2;
	temperature = ( ( BME->t_fine) * 5 + 128) >> 8;

	BME->temperature = temperature;

	BME->temperature_decimal = (temperature / 100) + BME->temp_calib;
	BME->temperature_fract = temperature % 100;
}

void BME280_calc_pressure(BME_sensor *BME)
{
	int64_t var1, var2;
	int64_t pressure;

	var1 = BME->t_fine - 128000;
	var2 = var1 * var1 * BME->calib_P[4];
	var2 = var2 + ((var1 * BME->calib_P[3]) << 17);
	var2 = var2 + ((int64_t)(BME->calib_P[2]) << 35);
	var1 = ((var1 * var1 * BME->calib_P[1]) >> 8) + ((var1 * BME->calib_P[0]) << 12);
	var1 = ((140737488355328) + var1) * BME->calib_P1 >> 33; //1 << 47 jako 1407...
	if(var1 == 0)
	{
		BME->pressure = 0;
		return;
	}
	pressure = 1048576 - BME->pressure_RAW;
	pressure = ((pressure << 31) - var2) * 3125 / var1;
	var1 = (int64_t)BME->calib_P[7] * (pressure >> 13) * (pressure >> 13) >> 25;
	var2 = (((int64_t)BME->calib_P[6]) * pressure) >> 19;

	pressure = ((pressure + var1 + var2) >> 8) + (((int64_t)BME->calib_P[5]) << 4);
	pressure = (uint32_t)(pressure >> 8);

	BME->pressure = pressure;

	BME->pressure_decimal = (pressure / 100) + BME->pressure_calib;
	BME->pressure_fract = pressure % 100;
}

void BME280_calc_humidity(BME_sensor *BME)
{
	//BME280_calc_temperature(BME);
	int32_t v_x1_u32r, humidity;

	v_x1_u32r = (BME->t_fine - ((int32_t)76800));
	v_x1_u32r = (((((BME->humidity_RAW << 14) - (((int32_t)BME->calib_H4) << 20)
			- (((int32_t)BME->calib_H5) * v_x1_u32r))
			+((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)BME->calib_H6)) >> 10) * (((v_x1_u32r * ((int32_t)BME->calib_H3)) >> 11)
			+((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)BME->calib_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)BME->calib_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
	v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;

	humidity = (v_x1_u32r >> 12);

	humidity =  (humidity / 1024.0) * 100;	//zamiast hum/1024 oraz hum %1024 - daje inny wynik po przecinku przy koncowce > 999

	BME->humidity = humidity;

	BME->humidity_decimal = (humidity / 100) + BME->humidity_calib;
	BME->humidity_fract = humidity % 100;
}

void BME280_load_temperature_calibration(BME_sensor *BME)
{
	uint8_t buffer[6];
	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, 0X88, 1, buffer, 6, HAL_MAX_DELAY);
	BME->calib_T1 = (buffer[1] << 8 ) | buffer[0];
	BME->calib_T2 = (buffer[3] << 8 ) | buffer[2];
	BME->calib_T3 = (buffer[5] << 8 ) | buffer[4];
}

void BME280_load_pressure_calibration(BME_sensor *BME)
{
	uint8_t buffer[18];
	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, 0X8E, 1, buffer, 18, HAL_MAX_DELAY);//9 rejestrow 16 bitowych
	BME->calib_P1 = (buffer[1] << 8) | buffer[0];
	for(uint8_t i=1; i < 9; i++)
	{
		BME->calib_P[i-1] = (buffer[ (i<<1) +1 ] << 8 ) | buffer[i << 1];
	}
}

void BME280_load_humidity_calibration(BME_sensor *BME)
{
	uint8_t buffer[7];
	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, 0xA1, 1, &BME->calib_H1, 1, HAL_MAX_DELAY);

	HAL_I2C_Mem_Read(&hi2c1, I2C_BME_ADDRESS, 0XE1, 1, buffer, 7, HAL_MAX_DELAY);

	BME->calib_H2 = (buffer[1] << 8 ) | buffer[0];
	BME->calib_H3 = buffer[2];
	BME->calib_H4 = (buffer[3] << 4 ) | (buffer[4] & 0x0F);
	BME->calib_H5 = (buffer[5] << 4) | ((buffer[4] & 0xF0) >> 4 );
	BME->calib_H6 = buffer[6];
}

void BME280_print_value(int16_t decimal, int16_t fract, const char *unit)
{
	usart_int16(decimal,10);
	usart_write_non_DMA(".");
	if(fract < 10)
	{
		usart_write_non_DMA("0");
	}
	usart_int16(fract,10);
	usart_write_non_DMA(" ");
	usart_write_non_DMA(unit);
}

void BME280_print_values_sensors(BME_sensor *BME)
{
	BME280_read_temperature_RAW(&BME->temperature_RAW);
	BME280_calc_temperature(BME);
	BME280_print_value(BME->temperature_decimal, BME->temperature_fract , "*C");

	usart_write_non_DMA("\t");

	BME280_read_pressure_RAW(&BME->pressure_RAW);
	BME280_calc_pressure(BME);
	BME280_print_value(BME->pressure_decimal, BME->pressure_fract, "hPa");

	usart_write_non_DMA("\t");

	BME280_read_humidity_RAW(&BME->humidity_RAW);
	BME280_calc_humidity(BME);
	BME280_print_value(BME->humidity_decimal, BME->humidity_fract, "%");
}


