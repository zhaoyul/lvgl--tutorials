#!/usr/bin/env python3
import subprocess
import os

# 所有需要的中文字符
chars = """、。一三下与两个中串为主义乐习了于些享从仓仪件优会位体作使例供侧便保信值储充先免入全公共关内凑函分切列创初到刷前力加动助区单卡即历原参口只史右号各合名后向和器回固图圆在块型域基填增声处复多夜大始媒字存学完官定实容密察对将小尔尽局层展左布带帧并序库应度建开式弧强当录形影径心态息悬慢成所打把折拉拖拟择拽持指按换据排接控播操支改放效教数整文新方旋无日时明是普景智曲更最有本机杂条来板构标栏样核格框案械模横次段每比波注流浮深清温源滑滚演点然片牙状率理用画界留的盖盘目知码础硬示种秒程空立章第等签简粗系紧级纯线组络统综缓网置考者背能自色节英菜蓝藏行表覆观角解计设访证说读调象负责起路轨轮辅输运返这进追适选逐通道遗避部配释重量钟钮键门问间阴阶隐集面音页项顺预题鲜齐（），：；"""

print(f"字符数: {len(chars)}")

# 生成字体 - 使用 Unicode 范围方式
for size in [14, 16]:
    output_file = f'src/fonts/lv_font_noto_{size}_cjk.c'
    
    # 使用 -r 指定 Unicode 范围
    cmd = [
        'lv_font_conv',
        '--font', 'src/fonts/NotoSansCJKsc-Regular.otf',
        '-r', '0x20-0x7f',           # ASCII
        '-r', '0x4e00-0x9fff',       # CJK 统一表意文字
        '-r', '0x3000-0x303f',       # CJK 标点
        '-r', '0xff00-0xffef',       # 全角字符
        '--size', str(size),
        '--bpp', '4',
        '--format', 'lvgl',
        '--lv-font-name', f'lv_font_noto_{size}_cjk',
        '-o', output_file
    ]
    
    print(f"\n生成 {size}px 字体...")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"错误: {result.stderr}")
    else:
        print(f"成功! 文件大小: {os.path.getsize(output_file) / 1024 / 1024:.1f}MB")

print("\n字体生成完成")
