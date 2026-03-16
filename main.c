/**
 * @file main.c
 * Cross-platform LVGL v9.2 desktop demo (macOS / Linux / Windows)
 *
 * Uses SDL2 as the display and input back-end so it runs anywhere SDL2 does.
 *
 * Build:
 *   cmake -B build -S . && cmake --build build
 *   ./build/bin/lvgl_demo
 *
 * Selectable demos (change DEMO_MODE below):
 *   0 — Custom dark-theme dashboard (default)
 *   1 — LVGL built-in widgets showcase
 *   2 — LVGL built-in music-player demo
 */

#include "lvgl.h"
#include <SDL2/SDL.h>
#include "src/ui_demo.h"

/* Pick which demo to show at startup:
 *   0 = custom dashboard
 *   1 = lv_demo_widgets()
 *   2 = lv_demo_music()     */
#ifndef DEMO_MODE
#define DEMO_MODE 0
#endif

#if DEMO_MODE == 1
#  include "demos/widgets/lv_demo_widgets.h"
#elif DEMO_MODE == 2
#  include "demos/music/lv_demo_music.h"
#endif

/* Window resolution -------------------------------------------------------- */
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  720

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /* 1. Initialise LVGL */
    lv_init();

    /* 2. Create an SDL2 window as the LVGL display */
    lv_display_t *disp = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_display_set_default(disp);

    /* 3. Create input devices */
    lv_indev_t *mouse      = lv_sdl_mouse_create();
    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t *keyboard   = lv_sdl_keyboard_create();

    /* 4. Default focus group for keyboard/encoder navigation */
    lv_group_t *group = lv_group_create();
    lv_group_set_default(group);
    lv_indev_set_group(keyboard, group);

    /* Suppress unused-variable warnings in case input devices aren't used */
    (void)mouse;
    (void)mousewheel;

    /* 5. Build the UI */
#if DEMO_MODE == 1
    lv_demo_widgets();
#elif DEMO_MODE == 2
    lv_demo_music();
#else
    ui_demo_create();
#endif

    /* 6. Main event loop — lv_timer_handler() drives everything.
     *    Window-close triggers exit() automatically via LV_SDL_DIRECT_EXIT. */
    while (1) {
        uint32_t delay_ms = lv_timer_handler();
        /* Clamp to a sane range so we don't spin or stall */
        if (delay_ms < 1)  delay_ms = 1;
        if (delay_ms > 16) delay_ms = 16;
        SDL_Delay(delay_ms);
    }

    return 0;
}
