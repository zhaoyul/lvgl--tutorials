/**
 * @file main.c
 * LVGL v9.2 桌面端教程入口（macOS / Linux / Windows）
 *
 * 本项目使用 SDL2 作为显示与输入后端，因此可以在支持 SDL2 的桌面平台运行。
 *
 * 构建方式：
 *   cmake -B build -S . && cmake --build build
 *   ./build/bin/lvgl_demo
 *
 * 可选演示模式（修改下方 DEMO_MODE）：
 *   0 —— 中文教程主页（默认）
 *   1 —— LVGL 官方 widgets 控件总览
 *   2 —— LVGL 官方 music 综合示例
 */

#include "lvgl.h"
#include <SDL2/SDL.h>
#include "src/ui_demo.h"

/* 选择启动时显示的内容：
 *   0 = 中文教程主页
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

/* 窗口分辨率 -------------------------------------------------------------- */
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  720

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /* 1. 初始化 LVGL */
    lv_init();

    /* 2. 创建 SDL2 窗口并作为 LVGL 显示设备 */
    lv_display_t *disp = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_display_set_default(disp);

    /* 3. 创建输入设备 */
    lv_indev_t *mouse      = lv_sdl_mouse_create();
    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t *keyboard   = lv_sdl_keyboard_create();

    /* 4. 设置默认焦点组，方便键盘/编码器导航 */
    lv_group_t *group = lv_group_create();
    lv_group_set_default(group);
    lv_indev_set_group(keyboard, group);

    /* 某些 demo 可能不会显式使用这些变量，这里避免未使用告警 */
    (void)mouse;
    (void)mousewheel;

    /* 5. 创建界面 */
#if DEMO_MODE == 1
    lv_demo_widgets();
#elif DEMO_MODE == 2
    lv_demo_music();
#else
    ui_demo_create();
#endif

    /* 6. 主事件循环：所有动画、输入与定时器都由 lv_timer_handler() 驱动。
     *    关闭窗口后，LV_SDL_DIRECT_EXIT 会自动退出进程。 */
    while (1) {
        uint32_t delay_ms = lv_timer_handler();
        /* 对延时做保护，避免空转或卡顿 */
        if (delay_ms < 1)  delay_ms = 1;
        if (delay_ms > 16) delay_ms = 16;
        SDL_Delay(delay_ms);
    }

    return 0;
}
