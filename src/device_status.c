#include "device_status.h"

#include <libdragon.h>

void device_status_scan(app_context_t *ctx)
{
    static bool sd_ready_cached = false;

    if (!ctx) return;

    if (!sd_ready_cached) {
        sd_ready_cached = debug_init_sdfs("sd:/", -1);
    }
    ctx->sd_ready = sd_ready_cached;

    eeprom_type_t eep = eeprom_present();
    ctx->eeprom_ready = (eep == EEPROM_4K || eep == EEPROM_16K);
    if (eep == EEPROM_4K) {
        ctx->eeprom_blocks = 64;
    } else if (eep == EEPROM_16K) {
        ctx->eeprom_blocks = 256;
    } else {
        ctx->eeprom_blocks = 0;
    }

    joypad_poll();
    JOYPAD_PORT_FOREACH(port) {
        ctx->cpak_ready[port] =
            joypad_get_accessory_type(port) == JOYPAD_ACCESSORY_TYPE_CONTROLLER_PAK &&
            validate_mempak(port) == 0;
    }
}
