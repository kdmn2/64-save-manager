#ifndef EEPROM_IO_H
#define EEPROM_IO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool eeprom_io_read_all(uint8_t **out_data, size_t *out_size);
bool eeprom_io_write_all(const uint8_t *data, size_t size);

#endif
