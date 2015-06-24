project(XFCE4_HOTCORNER_PLUGIN C)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(PkgConfig REQUIRED)

pkg_check_modules(XFCE4_PANEL libxfce4panel-2.0)
pkg_check_modules(GTK+ gtk+-3.0)

include_directories(${XFCE4_PANEL_INCLUDE_DIRS})
include_directories(${GTK+_INCLUDE_DIRS})

link_directories(${XFCE4_PANEL_LIBRARY_DIRS})
link_directories(${GTK+_LIBRARY_DIRS})

add_definitions(${XFCE4_PANEL_CFLAGS_OTHER})
add_definitions(${GTK+_CFLAGS_OTHER})

add_library(hotcorner SHARED src/hotcorner.c)
install(TARGETS hotcorner  LIBRARY DESTINATION lib/xfce4/panel/plugins/)
install(FILES hotcorner.desktop DESTINATION share/xfce4/panel/plugins/)
