#include "sd_store.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define SAVE_MAGIC 0x534D4752u
#define SAVE_VERSION 1u

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t version;
    uint16_t source;
    int32_t port;
    uint32_t payload_size;
    uint32_t checksum;
    char name[32];
} save_file_header_t;

static uint32_t fnv1a32(const uint8_t *data, size_t size)
{
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < size; i++) {
        hash ^= data[i];
        hash *= 16777619u;
    }
    return hash;
}

const char *sd_store_default_path(save_source_t source, int port)
{
    if (source == SAVE_SOURCE_EEPROM) return "sd:/savemgr_eeprom.sav";
    if (source == SAVE_SOURCE_CPAK) {
        switch (port) {
            case 0: return "sd:/savemgr_cpak_p1.sav";
            case 1: return "sd:/savemgr_cpak_p2.sav";
            case 2: return "sd:/savemgr_cpak_p3.sav";
            case 3: return "sd:/savemgr_cpak_p4.sav";
            default: return "sd:/savemgr_cpak.sav";
        }
    }
    return "sd:/savemgr_unknown.sav";
}

bool sd_store_save_blob(const save_blob_t *blob, char *out_path, size_t out_path_len)
{
    if (!blob || !blob->data || !blob->size) return false;

    const char *path = sd_store_default_path(blob->source, blob->port);
    FILE *fp = fopen(path, "wb");
    if (!fp) return false;

    save_file_header_t hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.magic = SAVE_MAGIC;
    hdr.version = SAVE_VERSION;
    hdr.source = (uint16_t)blob->source;
    hdr.port = blob->port;
    hdr.payload_size = (uint32_t)blob->size;
    hdr.checksum = fnv1a32(blob->data, blob->size);
    strncpy(hdr.name, blob->name, sizeof(hdr.name) - 1);

    bool ok = fwrite(&hdr, 1, sizeof(hdr), fp) == sizeof(hdr);
    ok = ok && fwrite(blob->data, 1, blob->size, fp) == blob->size;
    fclose(fp);

    if (!ok) return false;

    if (out_path && out_path_len) {
        strncpy(out_path, path, out_path_len - 1);
        out_path[out_path_len - 1] = '\0';
    }

    return true;
}

bool sd_store_load_blob(const char *path, save_blob_t *out_blob)
{
    if (!path || !out_blob) return false;

    FILE *fp = fopen(path, "rb");
    if (!fp) return false;

    save_file_header_t hdr;
    bool ok = fread(&hdr, 1, sizeof(hdr), fp) == sizeof(hdr);
    if (!ok || hdr.magic != SAVE_MAGIC || hdr.version != SAVE_VERSION || !hdr.payload_size) {
        fclose(fp);
        return false;
    }

    uint8_t *payload = malloc(hdr.payload_size);
    if (!payload) {
        fclose(fp);
        return false;
    }

    ok = fread(payload, 1, hdr.payload_size, fp) == hdr.payload_size;
    fclose(fp);
    if (!ok) {
        free(payload);
        return false;
    }

    if (fnv1a32(payload, hdr.payload_size) != hdr.checksum) {
        free(payload);
        return false;
    }

    memset(out_blob, 0, sizeof(*out_blob));
    out_blob->source = (save_source_t)hdr.source;
    out_blob->port = hdr.port;
    out_blob->data = payload;
    out_blob->size = hdr.payload_size;
    strncpy(out_blob->name, hdr.name, sizeof(out_blob->name) - 1);
    return true;
}
