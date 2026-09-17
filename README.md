# WPS图片批量嵌入工具

一个基于Qt 5.12和C++的桌面应用程序，用于将图片批量插入到WPS Excel文件中。

## 功能特性

- ✅ 从剪贴板粘贴图片
- ✅ 从文件添加图片
- ✅ 图片列表管理（添加、删除、排序）
- ✅ 批量插入图片到Excel文件
- ✅ 支持垂直和水平布局
- ✅ 直接编辑模式和剪贴板辅助模式
- ✅ 图片预览功能
- ✅ 自动保存临时文件

## 技术栈

- **GUI框架**: Qt 5.12
- **编程语言**: C++17
- **Python集成**: Python 3.x + openpyxl + Pillow
- **构建系统**: CMake 3.16+
- **平台**: Windows (优先)、Linux、macOS

## 系统架构

```
WPSImageTool/
├── include/              # 头文件
│   ├── ui/              # UI模块头文件
│   │   ├── MainWindow.h
│   │   ├── ImageListWidget.h
│   │   └── ClipboardManager.h
│   ├── core/            # 核心业务逻辑头文件
│   │   ├── ImageManager.h
│   │   ├── ImageProcessor.h
│   │   ├── ExcelController.h
│   │   └── DataStructures.h
│   └── python/          # Python集成头文件
│       └── PythonBridge.h
├── src/                 # 源文件
│   ├── main.cpp
│   ├── ui/              # UI实现
│   ├── core/            # 核心逻辑实现
│   └── python/          # Python桥接实现
├── scripts/             # Python脚本
│   └── excel_handler.py
├── resources/           # 资源文件
│   └── resources.qrc
└── CMakeLists.txt      # CMake配置
```

## 编译和安装

### 前置要求

1. **Qt 5.12或更高版本**
   ```bash
   # Windows
   下载并安装Qt: https://www.qt.io/download

   # Ubuntu/Debian
   sudo apt-get install qt5-default qtbase5-dev

   # macOS
   brew install qt@5
   ```

2. **CMake 3.16+**
   ```bash
   # Windows: 下载安装包
   # Ubuntu/Debian
   sudo apt-get install cmake

   # macOS
   brew install cmake
   ```

3. **Python 3.x**
   ```bash
   # Windows: 下载安装Python 3.8+
   # Ubuntu/Debian
   sudo apt-get install python3 python3-dev

   # macOS
   brew install python3
   ```

4. **Python依赖库**
   ```bash
   pip install openpyxl Pillow
   ```

### 编译步骤

```bash
# 1. 克隆或下载源码
cd WPSImageTool

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. 编译
cmake --build . --config Release

# 5. 安装（可选）
cmake --install .
```

### Windows特定配置

如果Qt未在系统PATH中：

```bash
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/5.12.0/msvc2017_64"
```

## 使用说明

### 基本流程

1. **打开Excel文件**
   - 点击"浏览"按钮选择要操作的.xlsx文件

2. **添加图片**
   - 方式1: 点击"粘贴图片"从剪贴板添加
   - 方式2: 点击"添加文件"从文件系统选择
   - 方式3: 直接按Ctrl+V粘贴

3. **设置目标单元格**
   - 在"起始单元格"输入框输入目标位置（如A1, B2）

4. **选择插入模式**
   - **直接编辑模式**: 自动操作Excel文件（需先关闭文件）
   - **剪贴板辅助模式**: 将图片放入剪贴板，手动粘贴

5. **插入图片**
   - 点击"插入图片到Excel"按钮

### 快捷键

- `Ctrl+V`: 从剪贴板粘贴图片
- `Ctrl+O`: 打开Excel文件
- `Delete`: 删除选中的图片
- `Ctrl+↑`: 上移选中图片
- `Ctrl+↓`: 下移选中图片

### 注意事项

⚠️ **直接编辑模式要求**:
- Excel文件必须关闭后才能操作
- 操作前程序会检测文件是否被占用
- 操作完成后可自动打开文件查看结果

⚠️ **图片格式支持**:
- PNG, JPG, JPEG, BMP, GIF, TIFF

⚠️ **单元格地址格式**:
- 正确: A1, B2, AA10, Z100
- 错误: 1A, a1 (小写), 1, A

## 设计模式与架构

### 模块化设计

- **UI层**: 负责用户界面和交互
- **业务逻辑层**: 处理图片管理、处理等核心功能
- **Python集成层**: 通过C++ Python API调用Python脚本

### 主要类

| 类名 | 职责 |
|------|------|
| `MainWindow` | 主窗口，协调各模块 |
| `ImageListWidget` | 图片列表控件 |
| `ClipboardManager` | 剪贴板监听和管理 |
| `ImageManager` | 图片数据管理 |
| `ImageProcessor` | 图片处理（缩放、转换等） |
| `ExcelController` | Excel操作控制器 |
| `PythonBridge` | C++与Python交互桥梁 |

### 数据流

```
用户操作 -> UI层 -> 业务逻辑层 -> Python集成层 -> Excel文件
          ↓
    ClipboardManager -> ImageManager -> ImageProcessor
                                      ↓
                               ExcelController -> PythonBridge -> excel_handler.py
```

## 代码规范

本项目遵循 **Google C++ Style Guide**:

- 命名规范:
  - 类名: `PascalCase` (如 `ImageManager`)
  - 函数: `PascalCase` (如 `GetImage`)
  - 变量: `snake_case` (如 `image_count_`)
  - 常量: `kPascalCase` (如 `kMaxWidth`)
  - 成员变量: `snake_case_` 后缀下划线

- 缩进: 2空格
- 头文件保护: `#ifndef FILENAME_H` + `#define FILENAME_H`
- 每行最大80-100字符

## 故障排除

### Python模块加载失败

**问题**: "加载Excel处理模块失败"

**解决方案**:
```bash
# 确认Python已安装
python --version

# 安装依赖
pip install openpyxl Pillow

# 检查scripts目录是否存在
ls scripts/excel_handler.py
```

### 无法打开Excel文件

**问题**: "文件正在被使用"

**解决方案**:
- 关闭WPS或Excel中的该文件
- 检查文件是否被其他程序锁定
- 重启WPS Office

### Qt库找不到

**问题**: 编译时找不到Qt库

**解决方案**:
```bash
# 设置Qt路径
export CMAKE_PREFIX_PATH=/path/to/qt5

# 或在CMake配置时指定
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt5
```

## 开发计划

### v1.0 (当前版本)
- [x] 基本图片管理功能
- [x] Excel文件操作
- [x] 直接编辑模式
- [x] 剪贴板辅助模式

### v1.1 (计划中)
- [ ] 图片批量压缩
- [ ] 自定义图片尺寸
- [ ] 网格布局模式
- [ ] 撤销/重做功能

### v1.2 (计划中)
- [ ] 多工作表支持
- [ ] 图片水印功能
- [ ] 批量处理多个文件
- [ ] 插件系统

## 许可证

Copyright © 2024 WPSImageTool

## 贡献

欢迎提交Issue和Pull Request！

## 联系方式

- 项目地址: [GitHub Repository]
- 问题反馈: [Issues]
