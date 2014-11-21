#include <avr/io.h> 
#include <stdio.h>

#ifndef F_CPU
	#define F_CPU 12000000UL 		//Your clock speed in Hz (3Mhz here)
#endif

#define LOOP_CYCLES 8 				//Number of cycles that the loop takes
#define us(num) (num/(LOOP_CYCLES*(1/(F_CPU/1000000.0))))

#define THERM_PORT PORTB
#define THERM_DDR DDRB
#define THERM_PIN PINB
#define THERM_DQ PB3
/* Utils */
#define THERM_INPUT_MODE() THERM_DDR&=~(1<<THERM_DQ)
#define THERM_OUTPUT_MODE() THERM_DDR|=(1<<THERM_DQ)
#define THERM_LOW() THERM_PORT&=~(1<<THERM_DQ)
#define THERM_HIGH() THERM_PORT|=(1<<THERM_DQ)
/* list of these commands translated into C defines:*/
#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec
/* constants */
#define THERM_DECIMAL_STEPS_12BIT 625 //.0625


void therm_delay(uint16_t delay);
uint8_t therm_reset();
void therm_write_bit(uint8_t bit);
uint8_t therm_read_bit(void);
uint8_t therm_read_byte(void);
void therm_write_byte(uint8_t byte);
//
void therm_read_temperature(char *buffer);
void therm_read_temperature2(int8_t *digi, uint16_t *deci);
