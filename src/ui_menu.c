#include "ui_menu.h"

#include <stdio.h>
#include <string.h>

#include <libdragon.h>

#include "app.h"
#include "device_status.h"
#include "save_manager.h"

typedef enum {
    ITEM_EEPROM_BACKUP = 0,
    ITEM_EEPROM_RESTORE,
    ITEM_CPAK1_BACKUP,
    ITEM_CPAK1_RESTORE,
    ITEM_RESCAN,
    ITEM_COUNT,
} menu_item_t;

static const char *menu_labels[ITEM_COUNT] = {
    "Backup EEPROM -> SD",
    "Restore EEPROM <- SD",
    "Backup CPAK P1 -> SD",
    "Restore CPAK P1 <- SD",
    "Rescan devices",
};

static void draw_status(const app_context_t *ctx, int cursor, const char *last_result)
{
    console_clear();
    printf("Save Manager MVP\n\n");
    printf("SD: %s\n", ctx->sd_ready ? "OK" : "Missing");
    printf("EEPROM: %s", ctx->eeprom_ready ? "OK" : "Missing");
    if (ctx->eeprom_ready) printf(" (%u blocks)", ctx->eeprom_blocks);
    printf("\n");
    printf("CPAK P1: %s\n\n", ctx->cpak_ready[0] ? "OK" : "Missing/Invalid");

    for (int i = 0; i < ITEM_COUNT; i++) {
        printf("%c %s\n", i == cursor ? '>' : ' ', menu_labels[i]);
    }

    printf("\nA: Run   DPad: Move\n");
    printf("Last: %s\n", last_result);
    console_render();
}

static bool can_run(const app_context_t *ctx, menu_item_t item, char *msg, int msg_len)
{
    if (!ctx->sd_ready && item != ITEM_RESCAN) {
        strncpy(msg, "SD not ready", msg_len - 1);
        msg[msg_len - 1] = '\0';
        return false;
    }
    if ((item == ITEM_EEPROM_BACKUP || item == ITEM_EEPROM_RESTORE) && !ctx->eeprom_ready) {
        strncpy(msg, "EEPROM missing", msg_len - 1);
        msg[msg_len - 1] = '\0';
        return false;
    }
    if ((item == ITEM_CPAK1_BACKUP || item == ITEM_CPAK1_RESTORE) && !ctx->cpak_ready[0]) {
        strncpy(msg, "CPAK P1 missing/invalid", msg_len - 1);
        msg[msg_len - 1] = '\0';
        return false;
    }
    return true;
}

void ui_menu_run(void)
{
    app_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    device_status_scan(&ctx);

    int cursor = 0;
    char last_result[96] = "Ready";

    while (1) {
        draw_status(&ctx, cursor, last_result);

        joypad_poll();
        joypad_buttons_t keys = joypad_get_buttons_pressed(JOYPAD_PORT_1);

        if (keys.d_up) {
            cursor = (cursor + ITEM_COUNT - 1) % ITEM_COUNT;
        }
        if (keys.d_down) {
            cursor = (cursor + 1) % ITEM_COUNT;
        }
        if (!keys.a) continue;

        menu_item_t item = (menu_item_t)cursor;
        if (item == ITEM_RESCAN) {
            device_status_scan(&ctx);
            strncpy(last_result, "Rescanned", sizeof(last_result) - 1);
            last_result[sizeof(last_result) - 1] = '\0';
            continue;
        }

        if (!can_run(&ctx, item, last_result, sizeof(last_result))) {
            continue;
        }

        bool ok = false;
        switch (item) {
            case ITEM_EEPROM_BACKUP:
                ok = save_manager_backup(SAVE_SOURCE_EEPROM, -1, last_result, sizeof(last_result));
                break;
            case ITEM_EEPROM_RESTORE:
                ok = save_manager_restore(SAVE_SOURCE_EEPROM, -1, last_result, sizeof(last_result));
                break;
            case ITEM_CPAK1_BACKUP:
                ok = save_manager_backup(SAVE_SOURCE_CPAK, 0, last_result, sizeof(last_result));
                break;
            case ITEM_CPAK1_RESTORE:
                ok = save_manager_restore(SAVE_SOURCE_CPAK, 0, last_result, sizeof(last_result));
                break;
            default:
                break;
        }

        if (!ok && !last_result[0]) {
            strncpy(last_result, "Operation failed", sizeof(last_result) - 1);
            last_result[sizeof(last_result) - 1] = '\0';
        }
    }
}
