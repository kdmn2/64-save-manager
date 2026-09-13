#include <libdragon.h>

#include "ui_menu.h"

int main(void)
{
    debug_init(DEBUG_FEATURE_LOG_EMU | DEBUG_FEATURE_LOG_USB | DEBUG_FEATURE_FILE_SD);
    console_init();
    joypad_init();
    timer_init();

    console_set_render_mode(RENDER_MANUAL);
    ui_menu_run();
    return 0;
}
