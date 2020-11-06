#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"

void i2c_init(void) {
	/*
		TWEN bit set in TWCR register to enable TWI(Two Wire Interface)
		TWSR register byte set to 0x00 for a prescaler of 1

		TWBR register byte set to 0x48 sets the bit rate to 100 kHz
		Calculation for TWBR:
			SCLfreq = F_CPU / (16 + (2 * TWBR * Prescaler))
			100 000 = 16 000 000 / (16 + (2 * TWBR * 1)
			TWBR = 72 = 0x48
	*/
	TWCR = (1 << TWEN);
	TWSR = 0x00;

	TWBR = 0x48;
}

void i2c_meaningful_status(uint8_t status) {
	/*
		Status of the TWI logic
	*/
	switch (status) {
		case 0x08: // START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("START\n"));
			break;
		case 0x10: // repeated START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("RESTART\n"));
			break;
		case 0x38: // NAK or DATA ARBITRATION LOST
			printf_P(PSTR("NOARB/NAK\n"));
			break;
		// MASTER TRANSMIT
		case 0x18: // SLA+W transmitted, ACK received
			printf_P(PSTR("MT SLA+W, ACK\n"));
			break;
		case 0x20: // SLA+W transmitted, NAK received
			printf_P(PSTR("MT SLA+W, NAK\n"));
				break;
		case 0x28: // DATA transmitted, ACK received
			printf_P(PSTR("MT DATA+W, ACK\n"));
			break;
		case 0x30: // DATA transmitted, NAK received
			printf_P(PSTR("MT DATA+W, NAK\n"));
			break;
		// MASTER RECEIVE
		case 0x40: // SLA+R transmitted, ACK received
			printf_P(PSTR("MR SLA+R, ACK\n"));
			break;
		case 0x48: // SLA+R transmitted, NAK received
			printf_P(PSTR("MR SLA+R, NAK\n"));
			break;
		case 0x50: // DATA received, ACK sent
			printf_P(PSTR("MR DATA+R, ACK\n"));
			break;
		case 0x58: // DATA received, NAK sent
			printf_P(PSTR("MR DATA+R, NAK\n"));
			break;
		default:
			printf_P(PSTR("N/A %02X\n"), status);
			break;
	}
}

inline void i2c_start() {
	/*
		TWEN bit set in TWCR register to enable SCL and SDA pins and enabling TWI operation
		TWSTA bit set in TWCR register to generate start condition, will be cleared in i2c_xmit_addr
		TWINT bit set in TWCR register clears the flag and starts the operation of TWI

		Await until TWI has finished its current job AKA TWINT bit is set
	*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); 

	while (!(TWCR & (1 << TWINT)));
}

inline void i2c_stop() {
	/*
		TWINT bit set in TWCR register clears the interrupt flag and starts the operation of TWI
		TWEN bit set in TWCR register to enable SCL and SDA pins and enabling TWI operation
		TWSTO bit set in TWCR register generates a stop condition on the 2-wire serial bus.
		After stop condition is executed the TWSTO bit is clearead automatically.
		
		Await until TWSTO bit is cleared
	*/
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

	while ((TWCR & (1 << TWSTO)));
}

inline uint8_t i2c_get_status(void) {
	/*
		Bits TWS3-7 in TWSR register reflect the status of the TWI logic
		Bits 0-2 are clearead to 0 in TWSR.
		status variabel is assigned value of TWSR.
	*/
	uint8_t status;
	status = TWSR & 0xF8;

	return status;
}

inline void i2c_xmit_addr(uint8_t eeprom_addr, uint8_t i2c_rw) {
	/*
		In Transmit mode, TWDR contains the next byte to be transmitted. In Receive mode, the TWDR contains
		the last byte received. 

		Register TWDR is assigned the eeprom address and the read/write bit

		TWINT bit set in TWCR register clears the interrupt flag and starts the operation of TWI
		TWEN bit set in TWCR register to enable SCL and SDA pins and enabling TWI operation

		Await until TWI has finished its current job AKA TWINT bit is set and ACK/NACK pulse has been sent
	*/
	
	TWDR = (eeprom_addr & 0xfe) | (i2c_rw & 0x01);

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)));
}

inline void i2c_xmit_byte(uint8_t byte) {
	/*
		TWINT bit set in TWCR register clears the interrupt flag and starts the operation of TWI
		TWEN bit set in TWCR register to enable SCL and SDA pins and enabling TWI operation
		
		Assign TWDR register with byte to send

		Await until TWI has finished its current job AKA TWINT bit is set and ACK/NACK pulse has been sent
	*/
	TWDR = byte;

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)));
}

inline uint8_t i2c_read_ACK() {
	/*
		TWINT bit set in TWCR register clears the interrupt flag and starts the operation of TWI
		TWEN bit set in TWCR register to enable SCL and SDA pins and enabling TWI operation
		TWEA bit set in TWCR register generates a ACK pulse on the TWI bus

		Await until TWI has finished its current job AKA TWINT bit is set and ACK/NACK pulse has been sent

		Received data byte in TWDR is returned
	*/
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT)));

	return TWDR;
}

inline uint8_t i2c_read_NAK() {
	/*
		TWEN bit set in TWCR register to enable SCL and SDA pins and enabling TWI operation
		TWINT bit set in TWCR register clears the interrupt flag and starts the operation of TWI

		Await until TWI has finished its current job AKA TWINT bit is set and ACK/NACK pulse has been sent

		Received data byte in TWDR is returned
	*/
	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)));

	return TWDR;
}

inline void eeprom_wait_until_write_complete() {
	/*
		If the device is still busy with the write cycle, no ACK will be returned. If the
		cycle is complete, the device will return the ACK and the master can then proceed
		with the next Read or Write command.

		Await until SLAW+W transmitted and ACK received.
	*/
	while (i2c_get_status() != 0x18) {
		i2c_start();
		i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	}
}

uint8_t eeprom_read_byte(uint8_t eeprom_addr_write) {
	/*
		Start communication
		Transmit address to the EEPROM memory and Write
		Transmit the location in memory

		Start communication
		Transmit address to the EEPROM memory and Read
		Read byte on location and save in variabel and send NACK

		Stop communication
	*/
	uint8_t readByte;

	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	i2c_xmit_byte(eeprom_addr_write);

	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_R);
	readByte = i2c_read_NAK();

	i2c_stop();

	return readByte;
}

void eeprom_write_byte(uint8_t eeprom_addr_write, uint8_t byte) {
	/*
		Start communication
		Transmit address to the EEPROM memory and Write

		Transmit the location in memory
		Transmit the byte to write

		Stop communication and make sure device is finished with writing.
	*/
	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);

	i2c_xmit_byte(eeprom_addr_write);
	i2c_xmit_byte(byte);


	i2c_stop();
	eeprom_wait_until_write_complete();

}

void eeprom_write_page(uint8_t eeprom_addr_write, char* eepromInput) {
	//If data is above 8 bytes the first bytes will be overwritten
	/*
		Start communication
		Transmit address to the EEPROM memory and Write
		Transmit the location in memory

		Write byte for byte, address incrementation being automatic from the EEPROM

		Stop communication and make sure device is finished with writing.
	*/
	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	i2c_xmit_byte(eeprom_addr_write);

	for (int i = 0; i < strlen(eepromInput); i++) {
		i2c_xmit_byte(eepromInput[i]);
	}

	i2c_stop();
	eeprom_wait_until_write_complete();
}

void eeprom_sequential_read(char *eepromOutput, uint8_t eeprom_addr_write, uint8_t len) {
	/*
		Start communication
		Transmit address to the EEPROM memory and Write
		Transmit the location in memory

		Start communication
		Transmit address to the EEPROM memory and Read

		Read byte for byte, assigning value on each index in char array
		Sending ACK for each byte, last byte NAK is sent

		Stop communication
	*/
	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	i2c_xmit_byte(eeprom_addr_write);

	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_R);
	
	for (int i = 0; i < (len - 1); i++) {
		eepromOutput[i] = i2c_read_ACK();
	}	
	eepromOutput[len - 1] = i2c_read_NAK();

	i2c_stop();

}
