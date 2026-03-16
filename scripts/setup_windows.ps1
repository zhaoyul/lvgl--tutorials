# setup_windows.ps1
# Install dependencies (MSYS2 + MinGW packages) and build lvgl_demo on Windows.
# Run from a standard PowerShell prompt:
#   powershell -ExecutionPolicy Bypass -File scripts\setup_windows.ps1

$ErrorActionPreference = "Stop"

$msys2Bin = "C:\msys64\usr\bin\bash.exe"
if (-not (Test-Path $msys2Bin)) {
    Write-Host "==> MSYS2 not found at C:\msys64." -ForegroundColor Red
    Write-Host "    Please install MSYS2 from https://www.msys2.org/ first," -ForegroundColor Red
    Write-Host "    then re-run this script." -ForegroundColor Red
    exit 1
}

Write-Host "==> Installing MinGW packages via MSYS2 pacman..."
& $msys2Bin -lc "pacman -S --needed --noconfirm mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 git"

$projectRoot = Split-Path -Parent $PSScriptRoot
$buildDir    = Join-Path $projectRoot "build"

Write-Host ""
Write-Host "==> Configuring with CMake (MinGW Makefiles)..."
& $msys2Bin -lc "cd '$(($projectRoot -replace '\\','/'))'  && cmake -B build -S . -G 'MinGW Makefiles' -DCMAKE_BUILD_TYPE=Release"

Write-Host ""
Write-Host "==> Building..."
& $msys2Bin -lc "cd '$(($projectRoot -replace '\\','/'))' && cmake --build build -j`$(nproc)"

Write-Host ""
Write-Host "==> Done!  Run the demo with:" -ForegroundColor Green
Write-Host "    $buildDir\bin\lvgl_demo.exe" -ForegroundColor Green
