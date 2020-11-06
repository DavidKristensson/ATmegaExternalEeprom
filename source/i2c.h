#ifndef _I2C_H_
#define _I2C_H_

#define I2C_R	1
#define I2C_W	0

#define EEPROM_ADDR 0xA0 // Binary: 1010 0000, Decimal: 160
#define EEPROM_ADDR_WRITE1 0x10 //Binary: 0001 0000, Decimal: 16
#define EEPROM_ADDR_WRITE2 0x18
#define EEPROM_ADDR_WRITE3 0x20
#define EEPROM_ADDR_WRITE4 0x28

void i2c_init(void);

void i2c_start(void);
void i2c_stop(void);

uint8_t i2c_get_status(void);
void i2c_meaningful_status(uint8_t status);

void i2c_xmit_addr(uint8_t eeprom_addr, uint8_t i2c_rw);
void i2c_xmit_byte(uint8_t byte);


uint8_t eeprom_read_byte(uint8_t eeprom_addr_write);
void eeprom_write_byte(uint8_t eeprom_addr_write, uint8_t byte);

void eeprom_wait_until_write_complete(void);

void eeprom_write_page(uint8_t addr, char* data);
void eeprom_sequential_read(char *eepromOutput, uint8_t eeprom_addr_write, uint8_t len);

#endif // _I2C_H_
