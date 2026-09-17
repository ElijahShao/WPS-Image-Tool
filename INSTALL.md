# WPS图片批量嵌入工具 - 安装指南

## 系统要求

### 最低配置
- 操作系统: Windows 10 / Ubuntu 18.04 / macOS 10.14
- 内存: 4GB RAM
- 磁盘空间: 200MB
- Qt版本: 5.12或更高
- Python版本: 3.7或更高

### 推荐配置
- 操作系统: Windows 11 / Ubuntu 22.04 / macOS 13
- 内存: 8GB RAM
- Qt版本: 5.15
- Python版本: 3.9+

## 依赖安装

### Windows

#### 1. 安装Qt
```powershell
# 下载Qt在线安装器
https://www.qt.io/download-qt-installer

# 安装时选择
- Qt 5.12.x (MSVC 2017 64-bit)
- Qt Creator (可选)
```

#### 2. 安装Python
```powershell
# 下载Python 3.9+
https://www.python.org/downloads/

# 安装时勾选 "Add Python to PATH"
```

#### 3. 安装CMake
```powershell
# 下载CMake
https://cmake.org/download/

# 或使用Chocolatey
choco install cmake
```

#### 4. 安装Python依赖
```powershell
pip install openpyxl Pillow
```

### Ubuntu/Debian

```bash
# 更新软件源
sudo apt update

# 安装Qt开发库
sudo apt install qt5-default qtbase5-dev qttools5-dev

# 安装Python和开发库
sudo apt install python3 python3-dev python3-pip

# 安装CMake
sudo apt install cmake

# 安装Python依赖
pip3 install openpyxl Pillow
```

### macOS

```bash
# 安装Homebrew（如果未安装）
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装Qt
brew install qt@5

# 安装Python
brew install python@3.9

# 安装CMake
brew install cmake

# 安装Python依赖
pip3 install openpyxl Pillow
```

## 编译安装

### 方法一：使用构建脚本（推荐）

#### Windows
```cmd
# 克隆或解压源码
cd WPSImageTool

# 运行构建脚本
build.bat

# 可执行文件位置
build\Release\WPSImageTool.exe
```

#### Linux/macOS
```bash
# 克隆或解压源码
cd WPSImageTool

# 添加执行权限
chmod +x build.sh

# 运行构建脚本
./build.sh

# 可执行文件位置
build/WPSImageTool
```

### 方法二：手动编译

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 配置CMake
# Windows (MSVC)
cmake .. -G "Visual Studio 16 2019" -A x64 ^
  -DCMAKE_PREFIX_PATH="C:/Qt/5.12.0/msvc2017_64"

# Linux
cmake .. -DCMAKE_BUILD_TYPE=Release

# macOS
cmake .. -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH="/usr/local/opt/qt@5"

# 3. 编译
cmake --build . --config Release

# 4. 安装（可选）
sudo cmake --install .
```

## 配置Python路径

### 如果Python未在系统PATH中

#### Windows
```cmd
set PATH=C:\Python39;C:\Python39\Scripts;%PATH%
```

#### Linux/macOS
```bash
export PATH="/usr/local/bin/python3:$PATH"
```

### 验证Python安装
```bash
python3 --version
pip3 list | grep openpyxl
pip3 list | grep Pillow
```

## 运行程序

### Windows
```cmd
cd build\Release
WPSImageTool.exe
```

### Linux
```bash
cd build
./WPSImageTool
```

### macOS
```bash
cd build
./WPSImageTool
```

## 常见问题

### Q1: CMake找不到Qt
**解决方案**:
```bash
cmake .. -DCMAKE_PREFIX_PATH="/path/to/qt5"
```

### Q2: Python模块导入失败
**解决方案**:
```bash
# 确保安装了依赖
pip3 install openpyxl Pillow

# 检查scripts目录
ls scripts/excel_handler.py
```

### Q3: 编译时Qt版本冲突
**解决方案**:
```bash
# 清理构建目录
rm -rf build
mkdir build
cd build

# 明确指定Qt路径
cmake .. -DCMAKE_PREFIX_PATH="/path/to/correct/qt5"
```

### Q4: Windows上缺少MSVC编译器
**解决方案**:
```
安装Visual Studio 2017或更高版本
选择"使用C++的桌面开发"工作负载
```

### Q5: Linux上缺少Qt开发库
**解决方案**:
```bash
sudo apt install qt5-default qtbase5-dev qttools5-dev-tools
```

## 验证安装

### 1. 检查可执行文件
```bash
# Windows
build\Release\WPSImageTool.exe --version

# Linux/macOS
build/WPSImageTool --version
```

### 2. 测试Python模块
```bash
cd tests
python3 test_excel_handler.py
```

### 3. 运行程序
- 启动程序
- 检查界面是否正常显示
- 尝试打开Excel文件
- 测试粘贴图片功能

## 卸载

### 使用CMake安装的版本
```bash
cd build
sudo cmake --uninstall
```

### 手动删除
```bash
# 删除可执行文件
rm -rf build/

# 删除配置文件（可选）
rm -rf ~/.config/WPSImageTool/
```

## 打包发布

### Windows (使用windeployqt)
```cmd
cd build\Release
windeployqt WPSImageTool.exe

REM 创建安装包（使用NSIS或Inno Setup）
```

### Linux (使用AppImage)
```bash
# 使用linuxdeployqt工具
linuxdeployqt WPSImageTool -appimage
```

### macOS (创建.app bundle)
```bash
macdeployqt WPSImageTool.app -dmg
```

## 技术支持

如遇到安装问题，请：

1. 查看[README.md](README.md)
2. 检查[故障排除部分](README.md#故障排除)
3. 提交Issue到项目仓库

---

祝使用愉快！
