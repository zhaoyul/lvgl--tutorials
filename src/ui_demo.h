/**
 * @file src/ui_demo.h
 * Public interface for the custom dashboard UI demo.
 */

#ifndef UI_DEMO_H
#define UI_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Build and display the dark-themed smart dashboard.
 * Call once after lv_init() and the display/input devices are created.
 */
void ui_demo_create(void);

#ifdef __cplusplus
}
#endif

#endif /* UI_DEMO_H */
