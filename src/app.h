#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    SAVE_SOURCE_EEPROM = 0,
    SAVE_SOURCE_CPAK,
} save_source_t;

typedef enum {
    APP_ACTION_BACKUP = 0,
    APP_ACTION_RESTORE,
} app_action_t;

typedef struct {
    bool sd_ready;
    bool eeprom_ready;
    size_t eeprom_blocks;
    bool cpak_ready[4];
} app_context_t;

typedef struct {
    save_source_t source;
    int port;
    uint8_t *data;
    size_t size;
    char name[32];
} save_blob_t;

#endif
