#ifndef AS3935_H_
#define AS3935_H_

#define AS3935_ADD0_PIN 1
#define AS3935_ADD1_PIN 1
#define AS3935_ADDRESS ( (AS3935_ADD1_PIN << 2) | (AS3935_ADD0_PIN << 1) )

#define AS3935_AFE_GAIN_BOOST_POWER_REG 					0x00
#define AS3935_NOISE_FLOOR_LEVEL_WATCHDOG_THRESHOLD_REG 	0x01
#define AS3935_LIGHTNING_SPIKE_REJECTION_REG				0x02
#define AS3935_LCO_FDIV_MASK_DIST_INTERUPT_REG				0x03
#define AS3935_ENERGY_LIGHTNING_LSB_REG						0x04
#define AS3935_ENERGY_LIGHTNING_MSB_REG						0x05
#define AS3935_ENERGY_LIGHTNING_MMSB_REG					0x06
#define AS3935_DISTANCE_REG									0x07
#define AS3935_DISP_REG 									0x08
#define AS3935_TRCO_CALIB_REG								0x3A
#define AS3935_SRCO_CALIB_REG								0x3B
#define AS3935_CALIB_RCO_REG 								0x3D

enum AS3935_INTERRUPT_NAME{INT_NOISE_LEVEL_TOO_HIGH = 0, INT_DISTURBER_DETECTED, INT_LIGHTNING};

enum AS3935_AFE_GAIN_BOOST{AS3935_AFE_OUTDOOR = 0x0E, AS3935_AFE_INDOOR = 0x12};

enum AS3935_MINIMUM_NUMBER_OF_LIGHTNING {AS3935_MIN_LIGHTNING_1 = 0, AS3935_MIN_LIGHTNING_5 = 1, AS3935_MIN_LIGHTNING_9 = 2, AS3935_MIN_LIGHTNING_16 = 3};

/* minimum number of lighting
+----+------+
| n  |[5..4]|
+----+------+
| 1  |  00  |
+----+------+
| 5  |  01  |
+----+------+
| 9  |  10  |
+----+------+
| 16 |  11  |
+----+------+
*/

void AS3935_init(void);

void AS3935_set_spike_rejection(uint8_t v);

void AS3935_set_minimum_number_lightning(uint8_t v);

void AS3935_set_AFE_gain_boost(uint8_t v);

void AS3935_set_noise_floor_level(uint8_t v);

void AS3935_set_watchdog_threshold(uint8_t v);

void AS3935_set_noise_floor_level(uint8_t v);

void AS3935_set_watchdog_threshold(uint8_t v);

void AS3935_set_mask_disturber(uint8_t v);

void AS3935_get_AFE_gain_boost(void);

void AS3935_get_noise_floor_level(void);

void AS3935_get_watchdog_threshold(void);

void AS3935_get_number_of_lightning(void);

void AS3935_get_spike_rejection(void);

void AS3935_print_parameters(void);

uint32_t AS3935_get_lightning_energy(void);	/*get energy of single lightning*/

uint8_t AS3935_get_distance(void);	/*get estimation distance in meters of single lightning*/

void AS3935_get_interrupt_reason(void);	/*get value from interrupt register and check what caused interrupt */

uint8_t AS3935_get_lightning_flag(void);

void AS3935_set_lightning_flag(void);

void AS3935_clear_lightning_flag(void);


#endif /* AS3935_H_ */
