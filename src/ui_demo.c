/**
 * @file src/ui_demo.c
 *
 * 本文件负责两部分内容：
 * 1. 中文教程主页：把 LVGL 官方 demo 与本仓库示例串成“从简单到复杂”的学习路径；
 * 2. 自定义仪表盘：展示布局、图表、动画、定时器与状态面板等综合能力。
 *
 * 设计目标：
 * - 尽量复用 LVGL 官方 widgets / music demo，保证控件覆盖更完整；
 * - 保留原有仪表盘示例，作为“中阶综合案例”；
 * - 所有新增注释使用简体中文，便于与 README 一起形成系统教程。
 */

#include "ui_demo.h"
#include "lvgl.h"
#include "demos/widgets/lv_demo_widgets.h"
#include "demos/music/lv_demo_music.h"
#include <time.h>
#include <stdlib.h>

/* ─── 配色方案 ──────────────────────────────────────────────────────────────
 * 参考 GitHub 深色主题，并增加更鲜明的强调色。                            */
#define C_BG         lv_color_hex(0x0d1117)   /* page background            */
#define C_CARD       lv_color_hex(0x161b22)   /* card / panel background    */
#define C_BORDER     lv_color_hex(0x30363d)   /* subtle border              */
#define C_ACCENT     lv_color_hex(0x58a6ff)   /* primary blue accent        */
#define C_GREEN      lv_color_hex(0x3fb950)   /* success / RAM              */
#define C_RED        lv_color_hex(0xf85149)   /* danger / temp              */
#define C_ORANGE     lv_color_hex(0xd29922)   /* warning / CPU              */
#define C_PURPLE     lv_color_hex(0xa371f7)   /* info / network             */
#define C_TEXT       lv_color_hex(0xf0f6fc)   /* primary text               */
#define C_SUBTEXT    lv_color_hex(0x8b949e)   /* secondary / muted text     */
#define C_CARD2      lv_color_hex(0x21262d)   /* alternate card background  */

/* ─── 全局状态：供页面构建函数与定时器回调共享 ─────────────────────────── */
static lv_obj_t *g_clock_label   = NULL;
static lv_obj_t *g_chart         = NULL;
static lv_chart_series_t *g_ser_cpu = NULL;
static lv_chart_series_t *g_ser_ram = NULL;
static lv_timer_t *g_clock_timer = NULL;
static lv_timer_t *g_chart_timer = NULL;
static lv_obj_t *g_overlay_btn   = NULL;

/* 教程页面保留官方英文控件命名，便于直接对照 LVGL 官网资料。 */
#define CN_FONT_BODY  (&lv_font_montserrat_14)
#define CN_FONT_TITLE (&lv_font_montserrat_20)

/* ─── 通用辅助函数 ─────────────────────────────────────────────────────── */

/** 创建带圆角与阴影的卡片 */
static lv_obj_t *make_card(lv_obj_t *parent, int32_t w, int32_t h)
{
    lv_obj_t *c = lv_obj_create(parent);
    lv_obj_set_size(c, w, h);
    lv_obj_set_style_bg_color(c, C_CARD, 0);
    lv_obj_set_style_bg_opa(c, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(c, 12, 0);
    lv_obj_set_style_border_color(c, C_BORDER, 0);
    lv_obj_set_style_border_width(c, 1, 0);
    lv_obj_set_style_shadow_width(c, 20, 0);
    lv_obj_set_style_shadow_color(c, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(c, LV_OPA_30, 0);
    lv_obj_set_style_pad_all(c, 0, 0);
    lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);
    return c;
}

/** 创建普通文字标签 */
static lv_obj_t *make_label(lv_obj_t *parent, const char *text,
                             const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_set_style_text_color(l, color, 0);
    return l;
}

/** 创建支持自动换行的教程说明文字 */
static lv_obj_t *make_wrap_label(lv_obj_t *parent, const char *text,
                                 const lv_font_t *font, lv_color_t color,
                                 int32_t width)
{
    lv_obj_t *l = make_label(parent, text, font, color);
    lv_label_set_long_mode(l, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(l, width);
    return l;
}

/**
 * 创建只读圆弧仪表卡片。
 * 返回卡片对象；圆弧会在进入页面时从 0 动画到目标值。
 */
static lv_obj_t *make_gauge(lv_obj_t *parent,
                              const char *title,
                              const char *value_str,
                              int32_t     value,
                              lv_color_t  color)
{
    lv_obj_t *card = make_card(parent, 220, 220);
    lv_obj_set_style_pad_top(card, 22, 0);
    lv_obj_set_style_pad_hor(card, 16, 0);
    lv_obj_set_style_pad_bottom(card, 14, 0);

    /* 圆弧主体 */
    lv_obj_t *arc = lv_arc_create(card);
    lv_obj_set_size(arc, 150, 150);
    lv_obj_align(arc, LV_ALIGN_TOP_MID, 0, 0);
    lv_arc_set_bg_angles(arc, 135, 45);   /* 270-degree sweep */
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 0);             /* start at 0; animate to value */

    /* 圆弧轨道（背景） */
    lv_obj_set_style_arc_color(arc, C_BORDER, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_MAIN);

    /* 圆弧指示器 */
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);

    /* 隐藏拖拽旋钮，改为纯展示模式 */
    lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);

    /* 数值标签，放在圆弧中心 */
    lv_obj_t *val_lbl = make_label(card, value_str,
                                    &lv_font_montserrat_32, C_TEXT);
    lv_obj_align_to(val_lbl, arc, LV_ALIGN_CENTER, 0, 0);

    /* 标题标签，位于圆弧下方 */
    lv_obj_t *ttl_lbl = make_label(card, title,
                                    &lv_font_montserrat_14, C_SUBTEXT);
    lv_obj_align_to(ttl_lbl, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

    /* 播放 0 → value 的进入动画 */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_arc_set_value);
    lv_anim_set_values(&a, 0, value);
    lv_anim_set_duration(&a, 1400);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    return card;
}

/* ─── 定时器回调 ───────────────────────────────────────────────────────── */

/** 每秒刷新一次标题栏时钟 */
static void clock_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if (!g_clock_label) return;
    time_t     now = time(NULL);
    struct tm *t   = localtime(&now);
    char       buf[64];
    strftime(buf, sizeof(buf), "%a %d %b    %H:%M:%S", t);
    lv_label_set_text(g_clock_label, buf);
}

/** 为历史曲线追加一组模拟数据 */
static void chart_update_cb(lv_timer_t *timer)
{
    (void)timer;
    if (!g_chart) return;
    /* 模拟缓慢波动的 CPU / RAM 数据 */
    static int32_t cpu = 65, ram = 48;
    cpu += (rand() % 11) - 5;
    ram += (rand() % 7)  - 3;
    if (cpu < 10) cpu = 10;
    if (cpu > 95) cpu = 95;
    if (ram < 20) ram = 20;
    if (ram > 85) ram = 85;
    lv_chart_set_next_value(g_chart, g_ser_cpu, (lv_value_precise_t)cpu);
    lv_chart_set_next_value(g_chart, g_ser_ram, (lv_value_precise_t)ram);
}

/* 清理当前页面遗留的定时器与悬浮按钮，避免切换 demo 后访问无效对象。 */
static void reset_runtime_state(void)
{
    if (g_clock_timer) {
        lv_timer_delete(g_clock_timer);
        g_clock_timer = NULL;
    }
    if (g_chart_timer) {
        lv_timer_delete(g_chart_timer);
        g_chart_timer = NULL;
    }
    if (g_overlay_btn) {
        lv_obj_delete(g_overlay_btn);
        g_overlay_btn = NULL;
    }

    g_clock_label = NULL;
    g_chart = NULL;
    g_ser_cpu = NULL;
    g_ser_ram = NULL;
}

/* ─── 仪表盘页面构建函数 ───────────────────────────────────────────────── */

static void build_header(lv_obj_t *parent)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, 1280, 58);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(bar, C_CARD, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(bar, C_BORDER, 0);
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_pad_hor(bar, 24, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    /* 左侧标题 */
    lv_obj_t *dot = lv_label_create(bar);
    lv_label_set_text(dot, LV_SYMBOL_WIFI "  LVGL Smart Dashboard");
    lv_obj_set_style_text_font(dot, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(dot, C_ACCENT, 0);
    lv_obj_align(dot, LV_ALIGN_LEFT_MID, 0, 0);

    /* 右侧实时时钟 */
    g_clock_label = make_label(bar, "--:--:--", &lv_font_montserrat_16, C_SUBTEXT);
    lv_obj_align(g_clock_label, LV_ALIGN_RIGHT_MID, 0, 0);
    clock_timer_cb(NULL);                              /* 先立即刷新一次 */
    g_clock_timer = lv_timer_create(clock_timer_cb, 1000, NULL);
}

static void build_gauge_row(lv_obj_t *parent)
{
    /* 三个仪表盘卡片使用 Flex 横向排布 */
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, 740, LV_SIZE_CONTENT);
    lv_obj_align(row, LV_ALIGN_TOP_LEFT, 16, 74);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_column(row, 16, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);

    make_gauge(row, "CPU Usage",   "65%",   65, C_ORANGE);
    make_gauge(row, "RAM Usage",   "48%",   48, C_ACCENT);
    make_gauge(row, "Temperature", "72 °C", 72, C_RED);
}

static void build_status_panel(lv_obj_t *parent)
{
    lv_obj_t *panel = make_card(parent, 464, 226);
    lv_obj_align(panel, LV_ALIGN_TOP_RIGHT, -16, 74);
    lv_obj_set_style_pad_all(panel, 20, 0);
    lv_obj_set_style_bg_color(panel, C_CARD2, 0);

    lv_obj_t *title = make_label(panel, "System Status",
                                  &lv_font_montserrat_16, C_TEXT);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    /* 右侧状态卡片列表 */
    static const struct {
        const char *icon;
        const char *label;
        const char *value;
        uint32_t    color;
        int32_t     y;
    } items[] = {
        { LV_SYMBOL_OK,       "System",    "Online",    0x3fb950, 36 },
        { LV_SYMBOL_CHARGE,   "FPS",       "60 fps",    0x58a6ff, 76 },
        { LV_SYMBOL_WIFI,     "Network",   "Excellent", 0xa371f7, 116 },
        { LV_SYMBOL_SETTINGS, "Uptime",    "3h 42m",    0xd29922, 156 },
    };

    for (int i = 0; i < 4; i++) {
        lv_obj_t *row = lv_obj_create(panel);
        lv_obj_set_size(row, LV_PCT(100), 30);
        lv_obj_align(row, LV_ALIGN_TOP_LEFT, 0, items[i].y);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 0, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *icon = make_label(row, items[i].icon,
                                     &lv_font_montserrat_16,
                                     lv_color_hex(items[i].color));
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);

        lv_obj_t *lbl = make_label(row, items[i].label,
                                    &lv_font_montserrat_14, C_SUBTEXT);
        lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 28, 0);

        lv_obj_t *val = make_label(row, items[i].value,
                                    &lv_font_montserrat_14,
                                    lv_color_hex(items[i].color));
        lv_obj_align(val, LV_ALIGN_RIGHT_MID, 0, 0);
    }
}

static void build_chart(lv_obj_t *parent)
{
    lv_obj_t *card = make_card(parent, 740, 200);
    lv_obj_align(card, LV_ALIGN_TOP_LEFT, 16, 316);
    lv_obj_set_style_pad_all(card, 14, 0);

    lv_obj_t *title = make_label(card, "CPU & RAM History",
                                  &lv_font_montserrat_14, C_SUBTEXT);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    /* 图例 */
    lv_obj_t *leg_cpu = make_label(card, "─ CPU",
                                    &lv_font_montserrat_12, C_ORANGE);
    lv_obj_align(leg_cpu, LV_ALIGN_TOP_RIGHT, -70, 0);

    lv_obj_t *leg_ram = make_label(card, "─ RAM",
                                    &lv_font_montserrat_12, C_ACCENT);
    lv_obj_align(leg_ram, LV_ALIGN_TOP_RIGHT, 0, 0);

    /* 折线图控件 */
    g_chart = lv_chart_create(card);
    lv_obj_set_size(g_chart, LV_PCT(100), 148);
    lv_obj_align(g_chart, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_chart_set_type(g_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(g_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_point_count(g_chart, 40);
    lv_chart_set_div_line_count(g_chart, 4, 8);

    lv_obj_set_style_bg_color(g_chart, C_CARD, 0);
    lv_obj_set_style_bg_opa(g_chart, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(g_chart, 0, 0);
    lv_obj_set_style_line_color(g_chart, C_BORDER, LV_PART_MAIN);
    lv_obj_set_style_line_opa(g_chart, LV_OPA_50, LV_PART_MAIN);
    /* 适度加粗折线，便于在深色背景中观察 */
    lv_obj_set_style_line_width(g_chart, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(g_chart, 0, 0, LV_PART_INDICATOR); /* hide dots */

    g_ser_cpu = lv_chart_add_series(g_chart, C_ORANGE,
                                     LV_CHART_AXIS_PRIMARY_Y);
    g_ser_ram = lv_chart_add_series(g_chart, C_ACCENT,
                                     LV_CHART_AXIS_PRIMARY_Y);

    /* 预填充一段更自然的历史数据 */
    static const int32_t cpu_hist[] = {
        55,60,72,68,75,80,70,65,72,78,75,68,72,80,76,72,70,75,78,80,
        72,68,75,80,82,78,74,72,68,75,78,82,80,76,72,74,78,75,80,65
    };
    static const int32_t ram_hist[] = {
        42,45,47,49,52,55,53,50,54,57,55,51,53,57,55,51,49,53,55,57,
        53,49,55,59,57,53,51,49,45,52,55,59,57,53,49,51,55,52,57,48
    };
    for (int i = 0; i < 40; i++) {
        lv_chart_set_next_value(g_chart, g_ser_cpu,
                                (lv_value_precise_t)cpu_hist[i]);
        lv_chart_set_next_value(g_chart, g_ser_ram,
                                (lv_value_precise_t)ram_hist[i]);
    }

    /* 每 800 ms 更新一次数据 */
    g_chart_timer = lv_timer_create(chart_update_cb, 800, NULL);
}

static void build_storage_panel(lv_obj_t *parent)
{
    lv_obj_t *card = make_card(parent, 464, 200);
    lv_obj_align(card, LV_ALIGN_TOP_RIGHT, -16, 316);
    lv_obj_set_style_pad_all(card, 20, 0);

    make_label(card, "Storage", &lv_font_montserrat_16, C_TEXT);

    static const struct {
        const char *name;
        int32_t     used;   /* percentage */
        uint32_t    color;
        int32_t     y;
    } drives[] = {
        { "SSD System",  67, 0x58a6ff, 34 },
        { "HDD Media",   42, 0x3fb950, 80 },
        { "SD Card  ",   88, 0xf85149, 126 },
    };

    for (int i = 0; i < 3; i++) {
        lv_obj_t *name = make_label(card, drives[i].name,
                                     &lv_font_montserrat_12, C_SUBTEXT);
        lv_obj_align(name, LV_ALIGN_TOP_LEFT, 0, drives[i].y);

        char pct[8];
        lv_snprintf(pct, sizeof(pct), "%"LV_PRId32"%%", drives[i].used);
        lv_obj_t *val = make_label(card, pct,
                                    &lv_font_montserrat_12,
                                    lv_color_hex(drives[i].color));
        lv_obj_align(val, LV_ALIGN_TOP_RIGHT, 0, drives[i].y);

        lv_obj_t *bar = lv_bar_create(card);
        lv_obj_set_size(bar, LV_PCT(100), 10);
        lv_obj_align(bar, LV_ALIGN_TOP_LEFT, 0, drives[i].y + 18);
        lv_bar_set_value(bar, drives[i].used, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(bar, C_BORDER, LV_PART_MAIN);
        lv_obj_set_style_bg_color(bar, lv_color_hex(drives[i].color),
                                   LV_PART_INDICATOR);
        lv_obj_set_style_radius(bar, 5, 0);
        lv_obj_set_style_radius(bar, 5, LV_PART_INDICATOR);

         /* 进度条进入动画 */
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, bar);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_bar_set_value);
        lv_anim_set_values(&a, 0, drives[i].used);
        lv_anim_set_duration(&a, 1200 + i * 200);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_start(&a);
    }
}

static void build_controls(lv_obj_t *parent)
{
    lv_obj_t *card = make_card(parent, 1248, 80);
    lv_obj_align(card, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_pad_hor(card, 24, 0);
    lv_obj_set_style_pad_ver(card, 12, 0);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_SPACE_EVENLY,
                           LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    static const struct {
        const char *icon;
        const char *name;
        bool        on;
        uint32_t    color;
    } controls[] = {
        { LV_SYMBOL_WIFI,      "Wi-Fi",         true,  0x58a6ff },
        { LV_SYMBOL_BLUETOOTH, "Bluetooth",      true,  0xa371f7 },
        { LV_SYMBOL_EYE_OPEN,  "Night Mode",     true,  0x3fb950 },
        { LV_SYMBOL_BELL,      "Notifications",  false, 0xd29922 },
        { LV_SYMBOL_VOLUME_MAX,"Sound",          true,  0x58a6ff },
    };

    for (int i = 0; i < 5; i++) {
        lv_obj_t *col = lv_obj_create(card);
        lv_obj_set_size(col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(col, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(col, 0, 0);
        lv_obj_set_style_pad_all(col, 4, 0);
        lv_obj_set_layout(col, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER,
                               LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(col, 4, 0);
        lv_obj_clear_flag(col, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *sw = lv_switch_create(col);
        if (controls[i].on) lv_obj_add_state(sw, LV_STATE_CHECKED);
        lv_obj_set_style_bg_color(sw, lv_color_hex(controls[i].color),
                                   LV_STATE_CHECKED | LV_PART_INDICATOR);

        lv_obj_t *icon = make_label(col, controls[i].icon,
                                     &lv_font_montserrat_12,
                                     lv_color_hex(controls[i].color));
        (void)icon;

        lv_obj_t *lbl = make_label(col, controls[i].name,
                                    &lv_font_montserrat_12, C_SUBTEXT);
        (void)lbl;
    }
}

/* ─── 教程主页 ─────────────────────────────────────────────────────────── */

static lv_obj_t *create_stage_card(lv_obj_t *parent, const char *step,
                                   const char *title, const char *summary,
                                   const char *topics, lv_event_cb_t cb,
                                   lv_color_t accent)
{
    lv_obj_t *card = make_card(parent, 392, 500);
    lv_obj_set_style_pad_all(card, 22, 0);
    lv_obj_set_style_bg_color(card, C_CARD2, 0);

    lv_obj_t *badge = lv_obj_create(card);
    lv_obj_set_size(badge, 108, 36);
    lv_obj_align(badge, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_radius(badge, 18, 0);
    lv_obj_set_style_border_width(badge, 0, 0);
    lv_obj_set_style_bg_color(badge, accent, 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_80, 0);
    lv_obj_clear_flag(badge, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *badge_label = make_label(badge, step, CN_FONT_BODY, C_TEXT);
    lv_obj_center(badge_label);

    lv_obj_t *title_label = make_wrap_label(card, title, CN_FONT_TITLE, C_TEXT, 348);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 0, 56);

    lv_obj_t *summary_label = make_wrap_label(card, summary, CN_FONT_BODY, C_SUBTEXT, 348);
    lv_obj_align(summary_label, LV_ALIGN_TOP_LEFT, 0, 112);

    lv_obj_t *topics_title = make_label(card, "What to learn", CN_FONT_BODY, accent);
    lv_obj_align(topics_title, LV_ALIGN_TOP_LEFT, 0, 206);

    lv_obj_t *topics_label = make_wrap_label(card, topics, CN_FONT_BODY, C_TEXT, 348);
    lv_obj_align(topics_label, LV_ALIGN_TOP_LEFT, 0, 238);

    lv_obj_t *btn = lv_button_create(card);
    lv_obj_set_size(btn, 170, 44);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_color(btn, accent, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = make_label(btn, "Open chapter", CN_FONT_BODY, C_TEXT);
    lv_obj_center(btn_label);

    return card;
}

static void show_tutorial_home(void);

static void back_to_home_cb(lv_event_t *e)
{
    (void)e;
    show_tutorial_home();
}

static void add_overlay_home_button(void)
{
    g_overlay_btn = lv_button_create(lv_layer_top());
    lv_obj_set_size(g_overlay_btn, 124, 40);
    lv_obj_align(g_overlay_btn, LV_ALIGN_TOP_LEFT, 16, 12);
    lv_obj_set_style_bg_color(g_overlay_btn, C_ACCENT, 0);
    lv_obj_set_style_shadow_width(g_overlay_btn, 12, 0);
    lv_obj_add_event_cb(g_overlay_btn, back_to_home_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = make_label(g_overlay_btn, "Back to home", CN_FONT_BODY, C_TEXT);
    lv_obj_center(label);
}

static void build_dashboard_demo(lv_obj_t *scr)
{
    lv_obj_set_style_bg_color(scr, C_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    build_header(scr);
    build_gauge_row(scr);
    build_status_panel(scr);
    build_chart(scr);
    build_storage_panel(scr);
    build_controls(scr);
}

static void show_dashboard_cb(lv_event_t *e)
{
    (void)e;
    reset_runtime_state();
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    build_dashboard_demo(scr);
    add_overlay_home_button();
}

static void show_widgets_cb(lv_event_t *e)
{
    (void)e;
    reset_runtime_state();
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_demo_widgets();
    add_overlay_home_button();
}

static void show_music_cb(lv_event_t *e)
{
    (void)e;
    reset_runtime_state();
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_demo_music();
    add_overlay_home_button();
}

static void show_tutorial_home(void)
{
    reset_runtime_state();

    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, C_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *hero = make_card(scr, 1248, 150);
    lv_obj_align(hero, LV_ALIGN_TOP_MID, 0, 16);
    lv_obj_set_style_pad_all(hero, 24, 0);

    lv_obj_t *title = make_wrap_label(
        hero,
        "LVGL tutorial path: learn UI from widgets to layouts and full applications",
        CN_FONT_TITLE, C_TEXT, 920);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *summary = make_wrap_label(
        hero,
        "This home page connects the official widgets demo, the dashboard in this repository, "
        "and the official music demo into one progressive learning path.",
        CN_FONT_BODY, C_SUBTEXT, 920);
    lv_obj_align(summary, LV_ALIGN_TOP_LEFT, 0, 42);

    lv_obj_t *tip = make_wrap_label(
        hero,
        "Notes in README and source code are written in Simplified Chinese. Widget names stay in English "
        "so they match the official LVGL documentation.",
        CN_FONT_BODY, C_ACCENT, 920);
    lv_obj_align(tip, LV_ALIGN_TOP_LEFT, 0, 100);

    lv_obj_t *path = make_wrap_label(
        hero,
        "Path: 01 Widgets overview -> 02 Dashboard layout & animation -> 03 Music application",
        CN_FONT_BODY, C_TEXT, 240);
    lv_obj_align(path, LV_ALIGN_TOP_RIGHT, 0, 18);

    lv_obj_t *cards = lv_obj_create(scr);
    lv_obj_set_size(cards, 1248, 520);
    lv_obj_align(cards, LV_ALIGN_BOTTOM_MID, 0, -14);
    lv_obj_set_style_bg_opa(cards, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cards, 0, 0);
    lv_obj_set_style_pad_all(cards, 0, 0);
    lv_obj_set_style_pad_column(cards, 18, 0);
    lv_obj_clear_flag(cards, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(cards, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cards, LV_FLEX_FLOW_ROW);

    create_stage_card(
        cards,
        "Chapter 1: Widgets",
        "Official widgets demo: survey the most common LVGL components",
        "Start from the official widget showcase and quickly understand how each widget looks and behaves.",
        "Focus widgets: Button, Label, Bar, Slider, Switch, Checkbox, Dropdown, Roller, Textarea, Keyboard, List, Table, Calendar, Chart, Tabview, Tileview, Menu, MsgBox, Spinner, Spinbox, Scale.",
        show_widgets_cb, C_ACCENT);

    create_stage_card(
        cards,
        "Chapter 2: Layouts and data",
        "Custom dashboard: learn Flex, charts, timers and animations",
        "After the widgets overview, study how multiple controls are combined into one dense information dashboard.",
        "Key topics: card layout, Arc gauges, Chart history view, Bar progress, Switch control groups, status panels, timer refresh and enter animations.",
        show_dashboard_cb, C_GREEN);

    create_stage_card(
        cards,
        "Chapter 3: Full application",
        "Official music demo: inspect a complete app-level UI",
        "Finally, study the music player demo to see page hierarchy, content composition and richer application flows.",
        "Key topics: complex layouts, page hierarchy, list and media controls, animation rhythm, unified theme and full application structure.",
        show_music_cb, C_PURPLE);
}

/* ─── 公共接口 ─────────────────────────────────────────────────────────── */

void ui_demo_create(void)
{
    srand((unsigned)time(NULL));
    show_tutorial_home();
}
