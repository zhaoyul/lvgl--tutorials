# lvgl-tutorials

> 完整的可以在 macOS / Linux / Windows 上运行的 LVGL 精美 UI 示例  
> Complete, cross-platform examples using [LVGL](https://github.com/lvgl/lvgl) that run on macOS, Linux, and Windows.

---

## 效果预览 / Preview

The default demo renders a **dark-themed smart dashboard** at 1280 × 720:

| Feature | Details |
|---------|---------|
| 3 animated arc gauges | CPU · RAM · Temperature |
| Live history chart | CPU & RAM with auto-scrolling data |
| Storage progress bars | Animated fill on startup |
| System status panel | FPS · Network · Uptime |
| Quick-control switches | Wi-Fi · Bluetooth · Sound, etc. |
| Live clock | Updated every second |
| Realtime FPS counter | Rendered by LVGL sysmon overlay |

You can also switch to the **built-in LVGL demos** by changing one `#define` in `main.c`:

| `DEMO_MODE` | Demo |
|-------------|------|
| `0` (default) | Custom dark dashboard |
| `1` | `lv_demo_widgets()` — widget showcase |
| `2` | `lv_demo_music()` — music-player UI |

---

## 依赖 / Prerequisites

| Dependency | Minimum version | Notes |
|-----------|----------------|-------|
| CMake | 3.16 | Build system |
| C compiler | C11 | GCC / Clang / MSVC |
| SDL2 | 2.0.14 | Display + input back-end |
| Internet (first build) | — | LVGL is fetched via `FetchContent` |

---

## 快速开始 / Quick Start

```bash
git clone https://github.com/zhaoyul/lvgl--tutorials.git
cd lvgl--tutorials

cmake -B build -S .
cmake --build build -j$(nproc)
./build/bin/lvgl_demo
```

---

## 平台安装指南 / Platform Setup

### 🐧 Linux (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install -y build-essential cmake libsdl2-dev
```

Then build and run:
```bash
cmake -B build -S .
cmake --build build -j$(nproc)
./build/bin/lvgl_demo
```

Or use the convenience script:
```bash
bash scripts/setup_linux.sh
```

---

### 🍎 macOS

Install Xcode Command Line Tools and Homebrew, then:
```bash
brew install cmake sdl2
```

Build and run:
```bash
cmake -B build -S .
cmake --build build -j$(sysctl -n hw.ncpu)
./build/bin/lvgl_demo
```

Or use:
```bash
bash scripts/setup_macos.sh
```

---

### 🪟 Windows

#### Option A — MSYS2 / MinGW (recommended)

1. Install [MSYS2](https://www.msys2.org/) and open an **MSYS2 MinGW 64-bit** shell.
2. Install dependencies:
   ```bash
   pacman -S --needed mingw-w64-x86_64-cmake \
                       mingw-w64-x86_64-gcc \
                       mingw-w64-x86_64-SDL2
   ```
3. Build:
   ```bash
   cmake -B build -S . -G "MinGW Makefiles"
   cmake --build build -j$(nproc)
   ./build/bin/lvgl_demo.exe
   ```

#### Option B — Visual Studio 2019/2022 with vcpkg

1. Install [vcpkg](https://vcpkg.io/):
   ```powershell
   git clone https://github.com/microsoft/vcpkg C:\vcpkg
   C:\vcpkg\bootstrap-vcpkg.bat
   C:\vcpkg\vcpkg install sdl2:x64-windows
   ```
2. Open a **Developer PowerShell** and build:
   ```powershell
   cmake -B build -S . `
         -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
         -DVCPKG_TARGET_TRIPLET=x64-windows
   cmake --build build --config Release
   .\build\bin\Release\lvgl_demo.exe
   ```

Or use the helper script (requires MSYS2 already installed):
```powershell
powershell -ExecutionPolicy Bypass -File scripts/setup_windows.ps1
```

---

## 项目结构 / Project Structure

```
lvgl-tutorials/
├── CMakeLists.txt      # Top-level CMake (fetches LVGL automatically)
├── lv_conf.h           # LVGL v9.2 configuration (SDL2, dark theme, demos)
├── main.c              # Entry point — choose demo via DEMO_MODE define
├── src/
│   ├── ui_demo.c       # Beautiful dark-theme dashboard UI
│   └── ui_demo.h       # Public API for the dashboard
├── scripts/
│   ├── setup_linux.sh   # One-click setup for Ubuntu/Debian
│   ├── setup_macos.sh   # One-click setup for macOS
│   └── setup_windows.ps1# One-click setup for Windows (MSYS2)
└── README.md
```

---

## 切换 Demo / Switching Demos

Edit the top of `main.c`:

```c
/* 0 = custom dashboard, 1 = lv_demo_widgets, 2 = lv_demo_music */
#define DEMO_MODE 0
```

Or pass it to CMake at configure time:
```bash
cmake -B build -S . -DCMAKE_C_FLAGS="-DDEMO_MODE=2"
cmake --build build
./build/bin/lvgl_demo     # opens the music-player demo
```

---

## 核心技术 / Technology

| Component | Details |
|-----------|---------|
| LVGL | v9.2.2 — fetched automatically via CMake `FetchContent` |
| SDL2 | Cross-platform display, mouse, keyboard, mousewheel |
| Fonts | Montserrat 12/14/16/20/24/32/48 |
| Theme | Dark (GitHub-inspired colour palette) |
| Animations | LVGL animation API — arc gauges, bar fills |
| Live data | `lv_timer_create()` — clock + chart updates |
| Flex layout | LVGL flex for responsive widget placement |

---

## 许可证 / License

LVGL is licensed under the [MIT License](https://github.com/lvgl/lvgl/blob/master/LICENCE.txt).  
This tutorial project is also MIT licensed.
