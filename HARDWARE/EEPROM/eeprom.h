/**
 * @file
 * @author dousha 
 * @date 2020/11/28.
 */

#ifndef CRYPTSTICK_EEPROM_H
#define CRYPTSTICK_EEPROM_H

#include "sys.h"

#define EEPROM_ADDR 0xa0u

#define EEPROM_BOOT_COUNT 0x10u
#define EEPROM_IV_ADDR 0x20u
#define EEPROM_SECRET_ADDR 0x40u

#define EEPROM_USE_ACCELERATED_PAGE_WRITE

extern void eeprom_init(I2C_HandleTypeDef *);

extern void eeprom_read(uint16_t, uint8_t *, uint16_t);

extern uint8_t eeprom_read_byte(uint16_t);

extern uint16_t eeprom_read_u16(uint16_t);

extern uint32_t eeprom_read_u32(uint16_t);

extern void eeprom_write_byte(uint16_t, uint8_t);

extern void eeprom_write_u16(uint16_t, uint16_t);

extern void eeprom_write_u32(uint16_t, uint32_t);

extern void eeprom_read_page(uint16_t, uint8_t *);

extern void eeprom_write_small_page(uint16_t, const uint8_t *, uint8_t);

extern void eeprom_write_page(uint16_t, uint8_t[32]);

extern void eeprom_seek(uint16_t);

extern void eeprom_wait_ready();

extern uint8_t eeprom_get_startup_count();

#endif //CRYPTSTICK_EEPROM_H