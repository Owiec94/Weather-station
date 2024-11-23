#ifndef BME280_H_
#define BME280_H_

#include <inttypes.h>

#define I2C_BME_ADDRESS 0xEC	//1110 1100, 1110110 << 1 = 0xEC
/*
	The 7-bit device address is 111011x. The 6 MSB bits are fixed. The last bit is changeable by
	SDO value and can be changed during operation. Connecting SDO to GND results in slave
	address 1110110 (0x76); connection it to VDDIO results in slave address 1110111 (0x77), which
	is the same as BMP280�s I2C address. The SDO pin cannot be left floating; if left floating, the
	I2C address will be undefined.
 */

#define BME280_WHO_AM_I_REG 0xD0
#define BME280_WHO_AM_I_VAL 0x60

//ctrl_hum register
#define BME280_CTRL_HUM_REG 0xF2


#define OVERSAMPLING_HUMIDITY_SKIPPED (0x00 << 0)
#define OVERSAMPLING_HUMIDITY_X1 (0x01 << 0)
#define OVERSAMPLING_HUMIDITY_X2 (0x02 << 0)
#define OVERSAMPLING_HUMIDITY_X4 (0x03 << 0)
#define OVERSAMPLING_HUMIDITY_X8 (0x04 << 0)
#define OVERSAMPLING_HUMIDITY_X16 (0x05 << 0)


//ctrl meas register
#define BME280_CTRL_MEAS_REG 0xF4

#define MODE_SLEEP_MODE 0x00
#define MODE_FORCED_MODE 0x01
#define MODE_NORMAL_MODE 0x03

#define OVERSAMPLING_PRESSURE_SKIPPED (0x00 << 2)
#define OVERSAMPLING_PRESSURE_X1 (0x01 << 2)
#define OVERSAMPLING_PRESSURE_X2 (0x02 << 2)
#define OVERSAMPLING_PRESSURE_X4 (0x03 << 2)
#define OVERSAMPLING_PRESSURE_X8 (0x04 << 2)
#define OVERSAMPLING_PRESSURE_X16 (0x05 << 2)

#define OVERSAMPLING_TEMPERATURE_SKIPPED (0x00 << 5)
#define OVERSAMPLING_TEMPERATURE_X1 (0x01 << 5)
#define OVERSAMPLING_TEMPERATURE_X2 (0x02 << 5)
#define OVERSAMPLING_TEMPERATURE_X4 (0x03 << 5)
#define OVERSAMPLING_TEMPERATURE_X8 (0x04 << 5)
#define OVERSAMPLING_TEMPERATURE_X16 (0x05 << 5)

//config register
#define BME_280_CONFIG_REG 0xF5

#define TIME_STANDBY_0_5 (0x00 << 5)
#define TIME_STANDBY_62_5 (0x01 << 5)
#define TIME_STANDBY_125 (0x02 << 5)
#define TIME_STANDBY_250 (0x03 << 5)
#define TIME_STANDBY_500 (0x04 << 5)
#define TIME_STANDBY_1000 (0x05 << 5)
#define TIME_STANDBY_10 (0x06 << 5)
#define TIME_STANDBY_20 (0x07 << 5)

#define FILTER_OFF (0x00 << 2)
#define FILTER_2 (0x01 << 2)
#define FILTER_4 (0x02 << 2)
#define FILTER_8 (0x03 << 2)
#define FILTER_16 (0x04 << 2)

#define DISABLE_SPI3W (0x00 << 0)
#define ENABLE_SPI3W (0x01 << 0)

typedef struct sensor
{
	int32_t temperature;
	int32_t temperature_RAW;
	int8_t temperature_decimal, temperature_fract;
	int32_t t_fine;
	uint8_t temp_calib;

	uint16_t calib_T1;
	int16_t calib_T2,calib_T3;


	int32_t pressure;
	int32_t pressure_RAW;
	int16_t pressure_decimal, pressure_fract;
	int8_t pressure_calib;
	uint16_t calib_P1;
	int16_t calib_P[8];


	int32_t humidity;
	int32_t humidity_RAW;
	int16_t humidity_decimal, humidity_fract;
	int8_t  humidity_calib;
	uint8_t calib_H1;
	int16_t calib_H2;
	uint8_t calib_H3;
	int16_t calib_H4;
	int16_t calib_H5;
	int8_t calib_H6;
}BME_sensor;

uint8_t BME280_check_connection(void);
uint8_t BME280_init(BME_sensor *BME280);
void BME280_calc_pressure(BME_sensor *BME);
void BME280_calc_temperature(BME_sensor *BME);
void BME280_calc_humidity(BME_sensor *BME);

void BME280_read_temperature_RAW(int32_t *temp);
void BME280_read_pressure_RAW(int32_t *pressure);
void BME280_read_humidity_RAW(int32_t *humidity);

void BME280_load_temperature_calibration(BME_sensor *BME);
void BME280_load_pressure_calibration(BME_sensor *BME);
void BME280_load_humidity_calibration(BME_sensor *BME);

void BME280_print_value(int16_t decimal, int16_t fract, const char *unit);
void BME280_print_values_sensors(BME_sensor *BME);


#endif /* BME280_H_ */
