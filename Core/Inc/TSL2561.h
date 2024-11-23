#ifndef TSL2561_H_
#define TSL2561_H_
#include <inttypes.h>

/*	0x29 - GND, 0x49 - VCC, 0x39 - FLOAT*/
#define I2C_TSL2561_ADRES 			(0x49 << 1)
#define TSL2561_CMD_BIT 			0x80

#define TSL2561_CONTROL_POWER_ON	0x03
#define TSL2561_CONTROL_POWER_OFF	0x00

#define TSL2561_TIMING_INTEG_13ms 	(0b00 << 0)	//13.7 ms	fast but low resolution
#define TSL2561_TIMING_INTEG_101ms 	(0b01 << 0)	//101  ms	medium resolution and speed
#define TSL2561_TIMING_INTEG_402ms 	(0b10 << 0)	//402  ms	16-bit data but slowest conversions

#define TSL2561_TIMING_MANUAL_ON	(0b1 << 3)
#define TSL2561_TIMING_MANUAL_OFF	(0b0 << 3)

#define TSL2561_TIMING_GAIN_1X 		(0b0 << 4)	//No gain ... use in bright light to avoid sensor saturation
#define TSL2561_TIMING_GAIN_16X 	(0b1 << 4)	//16x gain ... use in low light to boost sensitivity

#define TSL2561_INTERRUPT_INTR_DISABLE 	(0b00 << 4)
#define TSL2561_INTERRUPT_INTR_LEVEL	(0b01 << 4)
#define TSL2561_INTERRUPT_INTRSMBALERT 	(0b10 << 4)


#define TSL2561_LUX_SCALE 14 // scale by 2^14
#define TSL2561_RATIO_SCALE 9 // scale ratio by 2^9

#define TSL2561_CH_SCALE 10 // scale channel values by 2^10
#define TSL2561_CHSCALE_TINT0 0x7517 // 322/11 * 2^CH_SCALE
#define TSL2561_CHSCALE_TINT1 0x0fe7 // 322/81 * 2^CH_SCALE

//TMB-6 (DFN Dual Flat No-Lead) package
#define TSL2561_K1T 0x0040 // 0.125 * 2^RATIO_SCALE
#define TSL2561_B1T 0x01f2 // 0.0304 * 2^LUX_SCALE
#define TSL2561_M1T 0x01be // 0.0272 * 2^LUX_SCALE
#define TSL2561_K2T 0x0080 // 0.250 * 2^RATIO_SCALE
#define TSL2561_B2T 0x0214 // 0.0325 * 2^LUX_SCALE
#define TSL2561_M2T 0x02d1 // 0.0440 * 2^LUX_SCALE
#define TSL2561_K3T 0x00c0 // 0.375 * 2^RATIO_SCALE
#define TSL2561_B3T 0x023f // 0.0351 * 2^LUX_SCALE
#define TSL2561_M3T 0x037b // 0.0544 * 2^LUX_SCALE
#define TSL2561_K4T 0x0100 // 0.50 * 2^RATIO_SCALE
#define TSL2561_B4T 0x0270 // 0.0381 * 2^LUX_SCALE
#define TSL2561_M4T 0x03fe // 0.0624 * 2^LUX_SCALE
#define TSL2561_K5T 0x0138 // 0.61 * 2^RATIO_SCALE
#define TSL2561_B5T 0x016f // 0.0224 * 2^LUX_SCALE
#define TSL2561_M5T 0x01fc // 0.0310 * 2^LUX_SCALE
#define TSL2561_K6T 0x019a // 0.80 * 2^RATIO_SCALE
#define TSL2561_B6T 0x00d2 // 0.0128 * 2^LUX_SCALE
#define TSL2561_M6T 0x00fb // 0.0153 * 2^LUX_SCALE
#define TSL2561_K7T 0x029a // 1.3 * 2^RATIO_SCALE
#define TSL2561_B7T 0x0018 // 0.00146 * 2^LUX_SCALE
#define TSL2561_M7T 0x0012 // 0.00112 * 2^LUX_SCALE
#define TSL2561_K8T 0x029a // 1.3 * 2^RATIO_SCALE
#define TSL2561_B8T 0x0000 // 0.000 * 2^LUX_SCALE
#define TSL2561_M8T 0x0000 // 0.000 * 2^LUX_SCALE

uint8_t TSL2561_init(void);
unsigned int TSL2561_calculate_lux(unsigned int ch0, unsigned int ch1);
unsigned int TSL2561_read(void);
void TSL2561_print_value(void);


#endif /* TSL2561_H_ */
