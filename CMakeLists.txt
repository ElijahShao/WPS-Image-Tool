cmake_minimum_required(VERSION 3.16)
project(WPSImageTool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt5配置
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

# Python配置
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/ui
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/python
    ${Python3_INCLUDE_DIRS}
)

# 源文件
set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/ImageListWidget.cpp
    src/ui/ClipboardManager.cpp
    src/core/ImageManager.cpp
    src/core/ImageProcessor.cpp
    src/core/ExcelController.cpp
    src/python/PythonBridge.cpp
)

# 头文件
set(HEADERS
    include/ui/MainWindow.h
    include/ui/ImageListWidget.h
    include/ui/ClipboardManager.h
    include/core/ImageManager.h
    include/core/ImageProcessor.h
    include/core/ExcelController.h
    include/core/DataStructures.h
    include/python/PythonBridge.h
)

# 资源文件
set(RESOURCES
    resources/resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Python3::Python
)

# Windows特定设置
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 安装Python脚本
install(DIRECTORY scripts/
    DESTINATION bin/scripts
    FILES_MATCHING PATTERN "*.py"
)

# 安装可执行文件
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
