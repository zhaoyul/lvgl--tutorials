cmake_minimum_required(VERSION 3.16)
project(lvgl_tutorials C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

# ─── SDL2 ─────────────────────────────────────────────────────────────────────
# Windows: set SDL2_DIR env var, e.g. C:\SDL2-2.x.x\cmake  or pass -DSDL2_DIR=...
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "$ENV{SDL2_DIR}")
endif()

find_package(SDL2 REQUIRED)

# ─── LVGL (fetched automatically from GitHub) ─────────────────────────────────
include(FetchContent)

# Point LVGL to our lv_conf.h
set(LV_CONF_PATH "${CMAKE_SOURCE_DIR}/lv_conf.h" CACHE STRING "Path to lv_conf.h" FORCE)

FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG        v9.2.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(lvgl)

# Provide SDL2 headers and library to LVGL (needed to compile the built-in SDL driver)
if(TARGET SDL2::SDL2)
    target_link_libraries(lvgl PUBLIC SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
    target_link_libraries(lvgl PUBLIC SDL2::SDL2-static)
else()
    target_include_directories(lvgl PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(lvgl PUBLIC ${SDL2_LIBRARIES})
endif()

# ─── Main executable ──────────────────────────────────────────────────────────
add_executable(lvgl_demo
    main.c
    src/ui_demo.c
)

target_include_directories(lvgl_demo PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# Link LVGL core + demos library
target_link_libraries(lvgl_demo PRIVATE lvgl lvgl_demos)

# POSIX platforms need math and thread libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(lvgl_demo PRIVATE m pthread)
endif()

set_target_properties(lvgl_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
