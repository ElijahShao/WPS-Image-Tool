@echo off
REM WPSImageTool 构建脚本 (Windows)

echo === WPS图片批量嵌入工具 - 构建脚本 ===
echo.

REM 检查CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到CMake
    echo 请安装CMake: https://cmake.org/download/
    pause
    exit /b 1
)
echo [OK] CMake已安装

REM 检查Python
where python >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到Python
    echo 请安装Python 3: https://www.python.org/downloads/
    pause
    exit /b 1
)
echo [OK] Python已安装

REM 安装Python依赖
echo.
echo 安装Python依赖...
pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo 警告: 无法自动安装Python依赖
    echo 请手动安装: pip install openpyxl Pillow
)

REM 创建构建目录
echo.
echo 创建构建目录...
if not exist build mkdir build
cd build

REM 配置CMake (需要根据实际Qt路径修改)
echo.
echo 配置CMake...
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo.
    echo CMake配置失败！
    echo 如果Qt路径不正确，请修改CMAKE_PREFIX_PATH
    echo 例如: cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/5.12.0/msvc2017_64"
    pause
    exit /b 1
)

REM 编译
echo.
echo 开始编译...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo.
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo === 编译成功！ ===
echo.
echo 可执行文件位置: build\Release\WPSImageTool.exe
echo.
echo 运行程序:
echo   cd build\Release
echo   WPSImageTool.exe
echo.
pause
