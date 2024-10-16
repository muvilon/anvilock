cmake_minimum_required(VERSION 3.5)
project(anvilock C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -g")

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
pkg_check_modules(WAYLAND_SERVER REQUIRED wayland-server)
find_library(PAM_LIB pam REQUIRED)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)

add_executable(anvilock main.c)

target_include_directories(anvilock PRIVATE
    ${WAYLAND_CLIENT_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${XKBCOMMON_INCLUDE_DIRS}
)

target_link_libraries(anvilock
    ${WAYLAND_CLIENT_LIBRARIES}
    ${WAYLAND_SERVER_LIBRARIES}
    ${PAM_LIB}
    ${XKBCOMMON_LIBRARIES}
)
