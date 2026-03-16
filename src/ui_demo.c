/**
 * @file src/ui_demo.c
 *
 * Beautiful dark-theme "Smart Dashboard" built with LVGL v9.2.
 *
 * Layout (1280 × 720):
 *
 *  ┌──────────────────────────────────────────────────────────────────────┐
 *  │  ● LVGL Smart Dashboard                    Mon 16 Mar  12:34:56     │  ← header
 *  ├──────────────────────────────────────────────────────────────────────┤
 *  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌────────────────────────┐ │
 *  │  │  CPU    │  │  RAM    │  │  Temp   │  │     Status cards       │ │
 *  │  │  (arc)  │  │  (arc)  │  │  (arc)  │  │  FPS · Ping · Net      │ │
 *  │  │  65 %   │  │  48 %   │  │  72 °C  │  └────────────────────────┘ │
 *  │  └─────────┘  └─────────┘  └─────────┘                             │
 *  │  ┌──────────────────────────────────────────┐  ┌─────────────────┐ │
 *  │  │  CPU + RAM history (line chart)          │  │  Storage bars   │ │
 *  │  │                                          │  │  SSD · HDD · SD │ │
 *  │  └──────────────────────────────────────────┘  └─────────────────┘ │
 *  │  ┌──────────────────────────────────────────────────────────────── ┐│
 *  │  │  Quick controls: Wi-Fi · Bluetooth · DarkMode · Notifications   ││
 *  │  └───────────────────────────────────────────────────────────────── ┘│
 *  └──────────────────────────────────────────────────────────────────────┘
 */

#include "ui_demo.h"
#include "lvgl.h"
#include <time.h>
#include <stdlib.h>

/* ─── Colour palette ────────────────────────────────────────────────────────
 * Inspired by GitHub's dark theme with vibrant accent colours.             */
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

/* ─── Globals shared between UI builders and timer callbacks ────────────── */
static lv_obj_t *g_clock_label   = NULL;
static lv_obj_t *g_chart         = NULL;
static lv_chart_series_t *g_ser_cpu = NULL;
static lv_chart_series_t *g_ser_ram = NULL;

/* ─── Helpers ───────────────────────────────────────────────────────────── */

/** Create a rounded card with shadow */
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

/** Create a styled label */
static lv_obj_t *make_label(lv_obj_t *parent, const char *text,
                             const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_set_style_text_color(l, color, 0);
    return l;
}

/**
 * Create a non-interactive arc gauge card.
 * Returns the card.  The arc animates from 0 to @p value on startup.
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

    /* Arc */
    lv_obj_t *arc = lv_arc_create(card);
    lv_obj_set_size(arc, 150, 150);
    lv_obj_align(arc, LV_ALIGN_TOP_MID, 0, 0);
    lv_arc_set_bg_angles(arc, 135, 45);   /* 270-degree sweep */
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 0);             /* start at 0; animate to value */

    /* Arc track (background) */
    lv_obj_set_style_arc_color(arc, C_BORDER, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_MAIN);

    /* Arc indicator */
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 14, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);

    /* Hide the draggable knob */
    lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);

    /* Value label centred in the arc */
    lv_obj_t *val_lbl = make_label(card, value_str,
                                    &lv_font_montserrat_32, C_TEXT);
    lv_obj_align_to(val_lbl, arc, LV_ALIGN_CENTER, 0, 0);

    /* Title label below the arc */
    lv_obj_t *ttl_lbl = make_label(card, title,
                                    &lv_font_montserrat_14, C_SUBTEXT);
    lv_obj_align_to(ttl_lbl, arc, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

    /* Animate arc from 0 → value */
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

/* ─── Timer callbacks ───────────────────────────────────────────────────── */

/** Update the clock label every second */
static void clock_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    time_t     now = time(NULL);
    struct tm *t   = localtime(&now);
    char       buf[64];
    strftime(buf, sizeof(buf), "%a %d %b    %H:%M:%S", t);
    lv_label_set_text(g_clock_label, buf);
}

/** Append a new simulated data point to the history chart */
static void chart_update_cb(lv_timer_t *timer)
{
    (void)timer;
    if (!g_chart) return;
    /* Simulate gently-drifting CPU and RAM values */
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

/* ─── Section builders ──────────────────────────────────────────────────── */

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

    /* Dot + title */
    lv_obj_t *dot = lv_label_create(bar);
    lv_label_set_text(dot, LV_SYMBOL_WIFI "  LVGL Smart Dashboard");
    lv_obj_set_style_text_font(dot, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(dot, C_ACCENT, 0);
    lv_obj_align(dot, LV_ALIGN_LEFT_MID, 0, 0);

    /* Live clock (right-aligned) */
    g_clock_label = make_label(bar, "--:--:--", &lv_font_montserrat_16, C_SUBTEXT);
    lv_obj_align(g_clock_label, LV_ALIGN_RIGHT_MID, 0, 0);
    clock_timer_cb(NULL);                           /* populate immediately  */
    lv_timer_create(clock_timer_cb, 1000, NULL);    /* refresh every second  */
}

static void build_gauge_row(lv_obj_t *parent)
{
    /* Flex row container for the three gauges */
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

    /* Status item helper */
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

    /* Legend */
    lv_obj_t *leg_cpu = make_label(card, "─ CPU",
                                    &lv_font_montserrat_12, C_ORANGE);
    lv_obj_align(leg_cpu, LV_ALIGN_TOP_RIGHT, -70, 0);

    lv_obj_t *leg_ram = make_label(card, "─ RAM",
                                    &lv_font_montserrat_12, C_ACCENT);
    lv_obj_align(leg_ram, LV_ALIGN_TOP_RIGHT, 0, 0);

    /* Chart widget */
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
    /* Draw the line with a slight width for visibility */
    lv_obj_set_style_line_width(g_chart, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(g_chart, 0, 0, LV_PART_INDICATOR); /* hide dots */

    g_ser_cpu = lv_chart_add_series(g_chart, C_ORANGE,
                                     LV_CHART_AXIS_PRIMARY_Y);
    g_ser_ram = lv_chart_add_series(g_chart, C_ACCENT,
                                     LV_CHART_AXIS_PRIMARY_Y);

    /* Pre-fill with plausible historic data */
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

    /* Refresh data every 800 ms */
    lv_timer_create(chart_update_cb, 800, NULL);
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

        /* Animate bar fill */
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

/* ─── Public API ────────────────────────────────────────────────────────── */

void ui_demo_create(void)
{
    lv_obj_t *scr = lv_screen_active();
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
