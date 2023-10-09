cmake_minimum_required(VERSION 3.7)
project(MyDBUS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(libName MyDBUS)
set(serverName server)
set(appName sender)
file(GLOB SOURCES_SERVER "src/lib/internal/*.cpp")
file(GLOB SOURCES_APP "src/testapp/main.cpp")
file(GLOB SOURCES_LIB "src/lib/external/*.cpp")
file(GLOB INCLUDE_LIB_DIRS "src/lib/external")

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
include_directories(${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

add_compile_options(-Werror -Wall -Wextra)
include_directories(${INCLUDE_LIB_DIRS})

add_library(${libName} ${SOURCES_LIB})
add_executable(${serverName} ${SOURCES_SERVER})
add_executable(${appName} ${SOURCES_APP})

target_link_libraries(${libName} ${DBUS_LIBRARIES})
target_link_libraries(${serverName} ${DBUS_LIBRARIES})
target_link_libraries(${appName} ${libName})
