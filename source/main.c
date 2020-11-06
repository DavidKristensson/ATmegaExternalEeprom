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

		Five char arrays are written to EEPROM with write page function
		Sequential read function reads from EEPROM and concactenates eepromOutput string
		eepromOutput is printed as a string and as hex
	*/
	i2c_init();
	uart_init();
	sei();

	/*
	// Sub-task 1
	uint8_t eepromOutputByte;
	eeprom_write_byte(EEPROM_ADDR_WRITE, 'D');
	*/

	/*
	// Sub-task 2
	uint8_t counter = 0;
	eeprom_write_byte(EEPROM_ADDR_WRITE, 'D');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 1, 'A');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 2, 'V');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 3, 'I');
	eeprom_write_byte(EEPROM_ADDR_WRITE + 4, 'D');
	*/

	char eepromInput1[9] = "     Hej";
	char eepromInput2[9] = "  Oscar ";
	char eepromInput3[9] = " eller  ";
	char eepromInput4[9] = "Niclas! ";

	char eepromOutput[40] = "";

	uint8_t lenOfAllInputs = strlen(eepromInput1) + strlen(eepromInput2) + strlen(eepromInput3) + strlen(eepromInput4);

	eeprom_write_page(EEPROM_ADDR_WRITE1, eepromInput1);
	eeprom_write_page(EEPROM_ADDR_WRITE1 + strlen(eepromInput1), eepromInput2);
	eeprom_write_page(EEPROM_ADDR_WRITE1 + strlen(eepromInput1) + strlen(eepromInput2), eepromInput3);
	eeprom_write_page(EEPROM_ADDR_WRITE1 + strlen(eepromInput1) + strlen(eepromInput2) + strlen(eepromInput3), eepromInput4);

	eeprom_sequential_read(eepromOutput, EEPROM_ADDR_WRITE1, lenOfAllInputs);

	printf_P(PSTR("String:%s\nHex:        "), eepromOutput, eepromOutput);

	for (int i = 0; i < strlen(eepromOutput); i++) {
		printf_P(PSTR("%X "), eepromOutput[i]);
	}
	while (1) {
		/*
		// Sub-Task 2
		printf_P(PSTR("%c"), eeprom_read_byte(EEPROM_ADDR_WRITE + counter));
		counter++;
		if (counter == 5) {
			counter = 0;
			printf_P(PSTR("\n"));
		}
		*/
		
		/*
		// Sub-Task 1
		eepromOutputByte = eeprom_read_byte(EEPROM_ADDR_WRITE);

		printf_P(PSTR("%c\n"), eepromOutputByte);
		*/
	}
	return 0;
}

