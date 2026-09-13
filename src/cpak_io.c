#include "cpak_io.h"

#include <malloc.h>

#include <libdragon.h>

#define MEMPAK_SECTOR_COUNT 128

bool cpak_io_read_all(int port, uint8_t **out_data, size_t *out_size)
{
    if (!out_data || !out_size) return false;
    if (port < 0 || port > 3) return false;
    if (validate_mempak(port) != 0) return false;

    size_t total_size = MEMPAK_SECTOR_COUNT * MEMPAK_BLOCK_SIZE;
    uint8_t *buffer = malloc(total_size);
    if (!buffer) return false;

    for (int s = 0; s < MEMPAK_SECTOR_COUNT; s++) {
        if (read_mempak_sector(port, s, &buffer[s * MEMPAK_BLOCK_SIZE]) != 0) {
            free(buffer);
            return false;
        }
    }

    *out_data = buffer;
    *out_size = total_size;
    return true;
}

bool cpak_io_write_all(int port, const uint8_t *data, size_t size)
{
    if (!data) return false;
    if (port < 0 || port > 3) return false;
    if (validate_mempak(port) != 0) return false;

    size_t expected_size = MEMPAK_SECTOR_COUNT * MEMPAK_BLOCK_SIZE;
    if (size != expected_size) return false;

    for (int s = 0; s < MEMPAK_SECTOR_COUNT; s++) {
        if (write_mempak_sector(port, s, (uint8_t *)&data[s * MEMPAK_BLOCK_SIZE]) != 0) {
            return false;
        }
    }

    return true;
}
