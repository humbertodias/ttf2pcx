cmake_minimum_required(VERSION 3.16)
project(TTF2PCX VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Enable automoc for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/character.cpp
    src/previewwidget.cpp
)

set(HEADERS
    src/mainwindow.h
    src/character.h
    src/previewwidget.h
)

# Create executable
add_executable(TTF2PCX ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(TTF2PCX PRIVATE src)

# Link Qt libraries
target_link_libraries(TTF2PCX
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Copy executable to parent folder after build
add_custom_command(TARGET TTF2PCX POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TTF2PCX> ${CMAKE_SOURCE_DIR}
    COMMENT "Copying executable to parent folder..."
)