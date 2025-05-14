cmake_minimum_required(VERSION 3.16)
project(kkMusic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 让 Qt 自动找 ui 文件目录
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

# Qt 模块
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network)


# 源文件列表
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/lrcwidget.cpp
    src/searchwidget.cpp
)

# UI 文件列表
set(UI_FILES
    ui/mainwindow.ui
    ui/lrcwidget.ui
)

# 资源文件
set(RESOURCE_FILES
    resources/res.qrc
)

# 可执行程序
add_executable(kkMusic
    ${SOURCES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

# 链接 Qt 库
target_link_libraries(kkMusic PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
)

# 设置输出目录
set_target_properties(kkMusic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
