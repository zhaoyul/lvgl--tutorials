#!/bin/bash
# 下载 Noto Sans CJK 并转换为 LVGL 字体

set -e

FONT_DIR="$(dirname "$0")/../src/fonts"
mkdir -p "$FONT_DIR"

echo "下载 Noto Sans CJK..."
curl -L -o /tmp/NotoSansCJK.zip \
    "https://github.com/notofonts/noto-cjk/raw/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf"

echo "需要用 lv_font_conv 工具转换为 LVGL 格式:"
echo "npm i -g lv_font_conv"
echo ""
echo "转换命令示例："
echo "lv_font_conv --font /tmp/NotoSansCJKsc-Regular.otf -r 0x20-0x7f --symbols \"
学习路径示例演示控件仪表板音乐官方初学者入门重点标签按钮进度条滑块开关列表图表全面了解系统各种圆弧滚轮键盘表格日历菜单布局定时器动画组合信息密集复杂页面层级媒体主题标准简单"
\" --size 16 --format lvgl -o $FONT_DIR/lv_font_noto_16_cjk.c"
