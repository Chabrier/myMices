project(myMices)
cmake_minimum_required(VERSION 2.8.8)

##
## Find Qt5
##
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

##
## Qt5 includes
##
include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})

set(SRC main.cpp mouse.cpp MainWindow.cpp)
set(HEADERS main.h mouse.h MainWindow.hpp)
set(RESOURCES mice.qrc)

add_executable(myMouse ${SRC})
qt5_use_modules(myMouse Core Widgets)

install (TARGETS myMouse DESTINATION bin)
install (FILES ${HEADERS} DESTINATION include)
install (DIRECTORY images DESTINATION ressources)
