# lvgl-tutorials

> 一个面向初学者的 LVGL 中文教程工程.
> 教程内容参考 LVGL 官方资料, 并把官方 examples, 官方 demo 与自定义示例组织成" 从简单到复杂" 的完整学习路径.

---

## 这个仓库现在能做什么

程序启动后, 默认进入 **中文教程主页**, 并提供 4 个按难度递进的学习章节:

| 章节    | 内容                         | 适合学习什么                                     |
|---------|------------------------------|--------------------------------------------------|
| 第 1 章 | LVGL 官方 `examples/widgets` | 从最小的单控件示例开始入门                       |
| 第 2 章 | `lv_demo_widgets()`          | 系统浏览更完整的 LVGL 常用 UI 组件               |
| 第 3 章 | 自定义 Dashboard             | 学习 Flex 布局, 图表, 动画, 定时器和状态面板组织 |
| 第 4 章 | `lv_demo_music()`            | 查看更接近真实产品的应用级界面组合方式           |

这样安排的目的, 是把" 官网控件总览" 和" 项目实战案例" 串成一条连续教程:

1. 先从最小示例认识单个控件;
2. 再通过 widgets demo 系统浏览控件族;
3. 接着理解控件如何排版, 联动与刷新;
4. 最后观察完整应用界面的组织方式.

> 说明: 教程文档与代码注释使用简体中文; 控件名称沿用 LVGL 官方英文命名, 方便与官网文档逐项对照.

---

## 教程学习路线

### 第 1 章: 官方 examples 入门

先进入 LVGL 官方 `examples/widgets` 中更适合入门的单控件示例.

程序里已经把一组最适合初学者的 examples 整理成独立入口, 方便按顺序点开:

- `lv_example_label_1`
- `lv_example_button_1`
- `lv_example_bar_1`
- `lv_example_slider_1`
- `lv_example_checkbox_1`
- `lv_example_switch_1`
- `lv_example_dropdown_1`
- `lv_example_textarea_1`
- `lv_example_list_1`
- `lv_example_chart_1`
- `lv_example_tabview_1`

这一章重点关注:

- 单个控件的最小创建方式
- 最基础的状态, 样式与交互
- 从" 单控件" 过渡到" 容器 + 控件" 的思路

对应官网资料:

- LVGL Examples 总览: <https://docs.lvgl.io/master/examples.html>
- LVGL Widgets Examples: <https://docs.lvgl.io/master/examples/widgets/index.html>

### 第 2 章: Widgets Demo 总览

在看完单控件 example 之后, 再进入 LVGL 官方 `widgets` demo, 系统浏览更完整的控件集合.

这一章重点关注:

- Button / Label / Image
- Bar / Slider / Arc / Spinner / Spinbox / Scale
- Switch / Checkbox / Dropdown / Roller
- Textarea / Keyboard
- List / Table / Calendar / Chart
- Tabview / Tileview / Menu / MsgBox / Window

对应官网资料:

- LVGL Widgets 总览: <https://docs.lvgl.io/master/details/widgets/index.html>
- LVGL 官方 demo: <https://docs.lvgl.io/master/details/integration/overview/demos.html>

### 第 3 章: 布局, 数据与动画

本仓库内置了一个深色仪表盘示例, 用来演示如何把多个控件组合成一页完整界面.

这一章重点关注:

- 使用 `Flex` 组织卡片与面板
- 使用 `Arc` 制作仪表盘
- 使用 `Chart` 展示历史数据
- 使用 `Bar` 展示容量进度
- 使用 `Switch` 组织快速控制区
- 使用 `lv_timer_create()` 做实时刷新
- 使用 `lv_anim_t` 做进入动画

### 第 4 章: 应用级综合示例

最后查看 LVGL 官方 `music` demo, 理解更完整的产品界面结构.

这一章重点关注:

- 更复杂的布局层级
- 页面之间的组织方式
- 统一视觉风格与交互节奏
- 列表, 控制区与内容区的协同

---

## 为什么说它能" 完整展示 LVGL 的全部 UI 功能"

这个仓库没有重复手写所有控件, 而是采用了更适合教程维护的方式:

1. **入门示例**: 先整合 LVGL 官方 `examples/widgets`, 让初学者从单控件最小示例开始;
2. **控件覆盖**: 再复用 LVGL 官方 `widgets` demo, 尽可能完整地展示官方常用控件;
3. **工程组织**: 保留本仓库自定义 Dashboard, 解释布局, 图表, 动画, 定时器这些在真实项目里更常见的能力;
4. **综合案例**: 引入官方 `music` demo, 展示应用级 UI 结构;
5. **中文说明**: 通过中文 README 和中文代码注释, 把这些示例串成系统教程.

如果后续要继续扩展教程, 建议也按这个顺序增加:

1. 单控件示例;
2. 控件集合与状态切换;
3. 容器与布局;
4. 数据驱动页面;
5. 真实应用界面.

---

## 依赖环境

| 依赖项   | 最低版本     | 用途                          |
|----------|--------------|-------------------------------|
| CMake    | 3.16         | 构建系统                      |
| C 编译器 | C11          | GCC / Clang / MSVC            |
| SDL2     | 2.0.14       | 桌面端显示与输入后端          |
| 网络     | 首次构建需要 | 通过 `FetchContent` 拉取 LVGL |

---

## 快速开始

```bash
git clone https://github.com/zhaoyul/lvgl--tutorials.git
cd lvgl--tutorials

cmake -B build -S .
cmake --build build -j$(nproc)
./build/bin/lvgl_demo
```

运行后会进入教程主页, 点击页面中的章节按钮即可逐步浏览不同 examples / demo.

---

## 平台安装指南

### Linux(Ubuntu / Debian)

```bash
sudo apt update
sudo apt install -y build-essential cmake libsdl2-dev
```

然后构建并运行:

```bash
cmake -B build -S .
cmake --build build -j$(nproc)
./build/bin/lvgl_demo
```

也可以直接执行:

```bash
bash scripts/setup_linux.sh
```

### macOS

先安装 Xcode Command Line Tools 和 Homebrew, 然后执行:

```bash
brew install cmake sdl2
```

构建并运行:

```bash
cmake -B build -S .
cmake --build build -j$(sysctl -n hw.ncpu)
./build/bin/lvgl_demo
```

或者执行:

```bash
bash scripts/setup_macos.sh
```

### Windows

#### 方案 A: MSYS2 / MinGW(推荐)

1. 安装 [MSYS2](https://www.msys2.org/), 打开 **MSYS2 MinGW 64-bit** 终端.
2. 安装依赖:
   ```bash
   pacman -S --needed mingw-w64-x86_64-cmake \
                       mingw-w64-x86_64-gcc \
                       mingw-w64-x86_64-SDL2
   ```
3. 构建并运行:
   ```bash
   cmake -B build -S . -G "MinGW Makefiles"
   cmake --build build -j$(nproc)
   ./build/bin/lvgl_demo.exe
   ```

#### 方案 B: Visual Studio 2019/2022 + vcpkg

1. 安装 [vcpkg](https://vcpkg.io/):
   ```powershell
   git clone https://github.com/microsoft/vcpkg C:\vcpkg
   C:\vcpkg\bootstrap-vcpkg.bat
   C:\vcpkg\vcpkg install sdl2:x64-windows
   ```
2. 在 **Developer PowerShell** 中构建:
   ```powershell
   cmake -B build -S . `
         -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
         -DVCPKG_TARGET_TRIPLET=x64-windows
   cmake --build build --config Release
   .\build\bin\Release\lvgl_demo.exe
   ```

如果已经装好 MSYS2, 也可以直接运行:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/setup_windows.ps1
```

---

## 项目结构

```text
lvgl-tutorials/
├── CMakeLists.txt       # 顶层构建脚本, 会自动拉取 LVGL
├── lv_conf.h            # LVGL 配置: 字体, 控件, demo 开关
├── main.c               # 程序入口, 可切换启动 demo
├── src/
│   ├── ui_demo.c        # 中文教程主页 + 自定义仪表盘示例
│   └── ui_demo.h        # 教程主页公共接口
├── scripts/
│   ├── setup_linux.sh
│   ├── setup_macos.sh
│   └── setup_windows.ps1
└── README.md
```

---

## 切换启动 Demo

默认推荐直接使用教程主页:

```c
#define DEMO_MODE 0
```

如果你只想单独打开某个官方 demo, 也可以修改 `main.c` 顶部的 `DEMO_MODE`:

```c
/* 0 = 中文教程主页, 1 = 官方 widgets demo, 2 = 官方 music demo */
#define DEMO_MODE 0
```

也可以在 CMake 配置阶段直接传入:

```bash
cmake -B build -S . -DCMAKE_C_FLAGS="-DDEMO_MODE=2"
cmake --build build
./build/bin/lvgl_demo
```

---

## 核心技术点

| 组件   | 说明                                       |
|--------|--------------------------------------------|
| LVGL   | v9.2.2, 通过 CMake `FetchContent` 自动获取 |
| SDL2   | 跨平台显示, 鼠标, 键盘, 滚轮输入           |
| 字体   | Montserrat                                 |
| 主题   | 深色主题, 适合演示多卡片界面               |
| 动画   | `lv_anim_t` 用于仪表盘与进度条进入动画     |
| 定时器 | `lv_timer_create()` 用于时钟与图表数据刷新 |
| 布局   | `Flex` 用于组织教程卡片和仪表盘组件        |

---

## 许可证

LVGL 使用 [MIT License](https://github.com/lvgl/lvgl/blob/master/LICENCE.txt).
本教程项目同样采用 MIT License.
