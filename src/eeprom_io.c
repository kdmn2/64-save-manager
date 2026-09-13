#include "eeprom_io.h"

#include <malloc.h>
#include <string.h>

#include <libdragon.h>

bool eeprom_io_read_all(uint8_t **out_data, size_t *out_size)
{
    if (!out_data || !out_size) return false;

    size_t blocks = eeprom_total_blocks();
    if (!blocks) return false;

    size_t total_size = blocks * EEPROM_BLOCK_SIZE;
    uint8_t *buffer = malloc(total_size);
    if (!buffer) return false;

    memset(buffer, 0, total_size);
    eeprom_read_bytes(buffer, 0, total_size);

    *out_data = buffer;
    *out_size = total_size;
    return true;
}

bool eeprom_io_write_all(const uint8_t *data, size_t size)
{
    if (!data || !size) return false;

    size_t expected_size = eeprom_total_blocks() * EEPROM_BLOCK_SIZE;
    if (!expected_size || size != expected_size) return false;

    eeprom_write_bytes(data, 0, size);
    return true;
}
