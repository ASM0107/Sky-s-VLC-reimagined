cmake_minimum_required(VERSION 3.16)

project(SkyVLCReimagined_Pro VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Multimedia MultimediaWidgets OpenGLWidgets Concurrent REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/backend
    ${CMAKE_SOURCE_DIR}/ui
    ${CMAKE_SOURCE_DIR}/rendering
    ${CMAKE_SOURCE_DIR}/metadata
    ${CMAKE_SOURCE_DIR}/dsp
    ${CMAKE_SOURCE_DIR}/threading
    ${CMAKE_SOURCE_DIR}/settings
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB_RECURSE SRC_FILES 
    "src/*.cpp" 
    "backend/*.cpp" 
    "ui/*.cpp" 
    "rendering/*.cpp" 
    "metadata/*.cpp" 
    "dsp/*.cpp" 
    "threading/*.cpp" 
    "settings/*.cpp"
    "resources.qrc"
)

file(GLOB_RECURSE HDR_FILES 
    "include/*.h" 
    "backend/*.h" 
    "ui/*.h" 
    "rendering/*.h" 
    "metadata/*.h" 
    "dsp/*.h" 
    "threading/*.h" 
    "settings/*.h"
)

add_executable(SkyVLCReimagined
    ${SRC_FILES}
    ${HDR_FILES}
)

target_link_libraries(SkyVLCReimagined PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::OpenGLWidgets
    Qt6::Concurrent
)

file(COPY ${CMAKE_SOURCE_DIR}/styles DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/playlists DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
file(COPY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_BINARY_DIR}/assets)
