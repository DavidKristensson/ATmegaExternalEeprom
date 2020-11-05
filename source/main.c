#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "serial.h"
#include "timer.h"

int main(void) {
	/*
		Initalizing I2C and UART

		Enabling global interrupts

		Writing DAVID char by char to EEPROM
		Printing DAVID through UART by reading from EEPROM
	*/
	i2c_init();
	uart_init();
	sei();

	/*
	// Sub-task 1
	uint8_t eepromOutputByte;
	eeprom_write_byte(EEPROM_ADDR_WRITE, 'D');
	*/

	uint8_t counter = 0;
	eeprom_write_byte(EEPROM_ADDR_WRITE, 'D');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 1, 'A');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 2, 'V');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 3, 'I');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 4, 'D');
	while (1) {
		printf_P(PSTR("%c"), eeprom_read_byte(EEPROM_ADDR_WRITE + counter));
		counter++;
		if (counter == 5) {
			counter = 0;
			printf_P(PSTR("\n"));
		}
		
		/*
		// Sub-Task 1
		eepromOutputByte = eeprom_read_byte(EEPROM_ADDR_WRITE);

		printf_P(PSTR("%c\n"), eepromOutputByte);
		*/
	}
	return 0;
}

