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

	uint8_t eepromOutputByte;
	/*
		Initalizing I2C and UART

		Enabling global interrupts

		Writing a char to EEPROM
		Reading char from EEPROM and saving in variable and printing it
	*/
	i2c_init();
	uart_init();
	sei();

	eeprom_write_byte(EEPROM_ADDR_WRITE, 'D');
	while (1) {
		eepromOutputByte = eeprom_read_byte(EEPROM_ADDR_WRITE);

		printf_P(PSTR("%c\n"), eepromOutputByte);
	}
	return 0;
}

