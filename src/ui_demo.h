/**
 * @file src/ui_demo.h
 * 中文教程主页与自定义仪表盘示例的公共接口。
 */

#ifndef UI_DEMO_H
#define UI_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 创建教程主页。
 *
 * 默认页面会按照“从简单到复杂”的顺序介绍 LVGL：
 * 1. 官方 examples/widgets：从单控件最小示例入门；
 * 2. 官方 widgets demo：系统查看常用控件；
 * 3. 本仓库仪表盘 demo：学习布局、图表、动画与定时器；
 * 4. 官方 music demo：查看更完整的应用级界面组织方式。
 *
 * 请在 lv_init() 完成且显示/输入设备创建后调用一次。
 */
void ui_demo_create(void);

#ifdef __cplusplus
}
#endif

#endif /* UI_DEMO_H */
