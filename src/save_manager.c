#include "save_manager.h"

#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cpak_io.h"
#include "eeprom_io.h"
#include "sd_store.h"

static void set_status(char *status, int status_len, const char *msg)
{
    if (!status || status_len <= 0) return;
    strncpy(status, msg, status_len - 1);
    status[status_len - 1] = '\0';
}

static bool read_source(save_source_t source, int port, save_blob_t *out_blob)
{
    uint8_t *data = NULL;
    size_t size = 0;
    bool ok = false;

    if (source == SAVE_SOURCE_EEPROM) {
        ok = eeprom_io_read_all(&data, &size);
        if (ok) strncpy(out_blob->name, "EEPROM", sizeof(out_blob->name) - 1);
    } else if (source == SAVE_SOURCE_CPAK) {
        ok = cpak_io_read_all(port, &data, &size);
        if (ok) snprintf(out_blob->name, sizeof(out_blob->name), "CPAK_P%d", port + 1);
    }

    if (!ok) return false;
    out_blob->source = source;
    out_blob->port = port;
    out_blob->data = data;
    out_blob->size = size;
    return true;
}

static bool write_source(const save_blob_t *blob)
{
    if (blob->source == SAVE_SOURCE_EEPROM) {
        return eeprom_io_write_all(blob->data, blob->size);
    }
    if (blob->source == SAVE_SOURCE_CPAK) {
        return cpak_io_write_all(blob->port, blob->data, blob->size);
    }
    return false;
}

static bool verify_source(const save_blob_t *expected)
{
    save_blob_t readback;
    memset(&readback, 0, sizeof(readback));

    if (!read_source(expected->source, expected->port, &readback)) {
        return false;
    }

    bool same = readback.size == expected->size &&
        memcmp(readback.data, expected->data, expected->size) == 0;
    free(readback.data);
    return same;
}

bool save_manager_backup(save_source_t source, int port, char *status, int status_len)
{
    save_blob_t blob;
    memset(&blob, 0, sizeof(blob));

    if (!read_source(source, port, &blob)) {
        set_status(status, status_len, "Read source failed");
        return false;
    }

    char path[64];
    bool ok = sd_store_save_blob(&blob, path, sizeof(path));
    free(blob.data);

    if (!ok) {
        set_status(status, status_len, "Write SD failed");
        return false;
    }

    snprintf(status, status_len, "Backup OK: %s", path);
    return true;
}

bool save_manager_restore(save_source_t source, int port, char *status, int status_len)
{
    const char *path = sd_store_default_path(source, port);

    save_blob_t blob;
    memset(&blob, 0, sizeof(blob));

    if (!sd_store_load_blob(path, &blob)) {
        set_status(status, status_len, "Read backup failed");
        return false;
    }

    if (blob.source != source || blob.port != port) {
        free(blob.data);
        set_status(status, status_len, "Backup type mismatch");
        return false;
    }

    if (!write_source(&blob)) {
        free(blob.data);
        set_status(status, status_len, "Write target failed");
        return false;
    }

    if (!verify_source(&blob)) {
        free(blob.data);
        set_status(status, status_len, "Verify failed");
        return false;
    }

    free(blob.data);
    snprintf(status, status_len, "Restore OK: %s", path);
    return true;
}
