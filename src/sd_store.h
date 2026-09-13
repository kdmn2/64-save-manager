#ifndef SD_STORE_H
#define SD_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "app.h"

const char *sd_store_default_path(save_source_t source, int port);
bool sd_store_save_blob(const save_blob_t *blob, char *out_path, size_t out_path_len);
bool sd_store_load_blob(const char *path, save_blob_t *out_blob);

#endif
