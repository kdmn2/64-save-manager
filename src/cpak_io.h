#ifndef CPAK_IO_H
#define CPAK_IO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool cpak_io_read_all(int port, uint8_t **out_data, size_t *out_size);
bool cpak_io_write_all(int port, const uint8_t *data, size_t size);

#endif
