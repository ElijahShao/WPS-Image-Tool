#!/bin/bash
# WPSImageTool 构建脚本 (Linux/macOS)

set -e

echo "=== WPS图片批量嵌入工具 - 构建脚本 ==="
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查依赖
echo "检查依赖..."

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}错误: 未找到CMake${NC}"
    echo "请安装CMake: https://cmake.org/download/"
    exit 1
fi
echo -e "${GREEN}✓ CMake已安装${NC}"

# 检查Python
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}错误: 未找到Python 3${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Python 3已安装${NC}"

# 检查Qt
if [ -z "$CMAKE_PREFIX_PATH" ]; then
    echo -e "${YELLOW}警告: CMAKE_PREFIX_PATH未设置${NC}"
    echo "如果构建失败，请设置Qt路径:"
    echo "  export CMAKE_PREFIX_PATH=/path/to/qt5"
fi

# 安装Python依赖
echo ""
echo "安装Python依赖..."
pip3 install -r requirements.txt || {
    echo -e "${YELLOW}警告: 无法自动安装Python依赖${NC}"
    echo "请手动安装: pip3 install openpyxl Pillow"
}

# 创建构建目录
echo ""
echo "创建构建目录..."
mkdir -p build
cd build

# 配置CMake
echo ""
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release || {
    echo -e "${RED}CMake配置失败${NC}"
    exit 1
}

# 编译
echo ""
echo "开始编译..."
cmake --build . --config Release -j$(nproc) || {
    echo -e "${RED}编译失败${NC}"
    exit 1
}

echo ""
echo -e "${GREEN}=== 编译成功！ ===${NC}"
echo ""
echo "可执行文件位置: build/WPSImageTool"
echo ""
echo "运行程序:"
echo "  cd build"
echo "  ./WPSImageTool"
echo ""
#!/bin/bash
# WPSImageTool 构建脚本 (Linux/macOS)

set -e

echo "=== WPS图片批量嵌入工具 - 构建脚本 ==="
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查依赖
echo "检查依赖..."

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}错误: 未找到CMake${NC}"
    echo "请安装CMake: https://cmake.org/download/"
    exit 1
fi
echo -e "${GREEN}✓ CMake已安装${NC}"

# 检查Python
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}错误: 未找到Python 3${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Python 3已安装${NC}"

# 检查Qt
if [ -z "$CMAKE_PREFIX_PATH" ]; then
    echo -e "${YELLOW}警告: CMAKE_PREFIX_PATH未设置${NC}"
    echo "如果构建失败，请设置Qt路径:"
    echo "  export CMAKE_PREFIX_PATH=/path/to/qt5"
fi

# 安装Python依赖
echo ""
echo "安装Python依赖..."
pip3 install -r requirements.txt || {
    echo -e "${YELLOW}警告: 无法自动安装Python依赖${NC}"
    echo "请手动安装: pip3 install openpyxl Pillow"
}

# 创建构建目录
echo ""
echo "创建构建目录..."
mkdir -p build
cd build

# 配置CMake
echo ""
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release || {
    echo -e "${RED}CMake配置失败${NC}"
    exit 1
}

# 编译
echo ""
echo "开始编译..."
cmake --build . --config Release -j$(nproc) || {
    echo -e "${RED}编译失败${NC}"
    exit 1
}

echo ""
echo -e "${GREEN}=== 编译成功！ ===${NC}"
echo ""
echo "可执行文件位置: build/WPSImageTool"
echo ""
echo "运行程序:"
echo "  cd build"
echo "  ./WPSImageTool"
echo ""
