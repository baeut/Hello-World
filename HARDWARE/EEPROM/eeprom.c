/**
 * @file
 * @author dousha 
 * @date 2020/11/28.
 */

#include "eeprom.h"
#include "buttons.h"
#include "../Logic/convert.h"
#include <string.h>

static I2C_HandleTypeDef *i2c;
static uint8_t buf[34] = {0};

void eeprom_prepare_address(uint16_t offset) {
	buf[0] = (uint8_t) (offset >> 8u);
	buf[1] = (uint8_t) (offset & 0xffu);
}

void eeprom_init(I2C_HandleTypeDef *i) {
	i2c = i;
	if (buttons_get_immediate_state() == BUTTON_LEFT_AND_RIGHT) {
		// reset boot counter (debug purpose)
		eeprom_write_byte(EEPROM_BOOT_COUNT, 0);
	} else {
		uint8_t count = eeprom_read_byte(EEPROM_BOOT_COUNT);
		++count;
		eeprom_write_byte(EEPROM_BOOT_COUNT, count);
	}
}

void eeprom_seek(uint16_t offset) {
	eeprom_prepare_address(offset);
	HAL_I2C_Master_Transmit(i2c, EEPROM_ADDR, buf, 2, HAL_MAX_DELAY);
	//eeprom_wait_ready();
}

uint8_t eeprom_read_byte(uint16_t offset) {
	eeprom_seek(offset);
	HAL_I2C_Master_Receive(i2c, EEPROM_ADDR, buf, 1, HAL_MAX_DELAY);
	return buf[0];
}

uint16_t eeprom_read_u16(uint16_t offset) {
	eeprom_seek(offset);
	HAL_I2C_Master_Receive(i2c, EEPROM_ADDR, buf, 2, HAL_MAX_DELAY);
	return get_u16(buf);
}

uint32_t eeprom_read_u32(uint16_t offset) {
	eeprom_seek(offset);
	HAL_I2C_Master_Receive(i2c, EEPROM_ADDR, buf, 4, HAL_MAX_DELAY);
	return get_u32(buf);
}

void eeprom_write_byte(uint16_t offset, uint8_t byte) {
	eeprom_prepare_address(offset);
	buf[2] = byte;
	HAL_I2C_Master_Transmit(i2c, EEPROM_ADDR, buf, 3, HAL_MAX_DELAY);
	eeprom_wait_ready();
}

void eeprom_write_u16(uint16_t offset, uint16_t value) {
	set_u16(buf + 3, value);
	eeprom_write_byte(offset, buf[3]);
	eeprom_write_byte(offset + 1, buf[4]);
}

void eeprom_write_u32(uint16_t offset, uint32_t value) {
	set_u32(buf + 3, value);
	for (uint8_t i = 0; i < 4; i++) {
		eeprom_write_byte(offset + i, buf[3 + i]);
	}
}

void eeprom_read(uint16_t offset, uint8_t *out, uint16_t length) {
	eeprom_seek(offset);
	HAL_I2C_Master_Receive(i2c, EEPROM_ADDR, out, length, HAL_MAX_DELAY);
}

void eeprom_read_page(uint16_t offset, uint8_t *page) {
	eeprom_seek(offset);
	HAL_I2C_Master_Receive(i2c, EEPROM_ADDR, page, 32, HAL_MAX_DELAY);
}

void eeprom_write_small_page(uint16_t offset, const uint8_t *data, uint8_t length) {
	if (length > 32) {
		return;
	}
	eeprom_prepare_address(offset);
	memcpy(buf + 2, data, length);
	HAL_I2C_Master_Transmit(i2c, EEPROM_ADDR, buf, 2 + length, HAL_MAX_DELAY);
	eeprom_wait_ready();
}

void eeprom_write_page(uint16_t offset, uint8_t page[32]) {
#ifdef EEPROM_USE_ACCELERATED_PAGE_WRITE
	eeprom_prepare_address(offset);
	memcpy(buf + 2, page, 32);
	if (HAL_I2C_Master_Transmit(i2c, EEPROM_ADDR, buf, 34, HAL_MAX_DELAY) != HAL_OK) {
		HAL_I2C_GetError(i2c);
		for (;;);
	}
	eeprom_wait_ready();
#else
	for (uint8_t i = 0; i < 32; i++) {
		eeprom_write_byte(offset + i, page[i]);
	}
#endif
}

uint8_t eeprom_get_startup_count() {
	return eeprom_read_byte(EEPROM_BOOT_COUNT);
}

void eeprom_wait_ready() {
	while (HAL_I2C_Master_Transmit(i2c, EEPROM_ADDR, buf, 0, 10) != HAL_OK) ;
}