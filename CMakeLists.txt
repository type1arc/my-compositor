cmake_minimum_required(VERSION 3.16)
project(wlcomp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WLROOTS REQUIRED wlroots-0.19 IMPORTED_TARGET)

find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

set(XDG_SHELL_XML "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml")
set(GENERATED_PROTOCOL_DIR "${CMAKE_CURRENT_BINARY_DIR}/protocol")

add_custom_command(
    OUTPUT ${GENERATED_PROTOCOL_DIR}/xdg-shell-protocol.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_PROTOCOL_DIR}
    COMMAND ${WAYLAND_SCANNER} server-header ${XDG_SHELL_XML}
            ${GENERATED_PROTOCOL_DIR}/xdg-shell-protocol.h
    DEPENDS ${XDG_SHELL_XML}
    COMMENT "Generating xdg-shell-protocol.h"
)

add_custom_target(generate-protocol-headers
    DEPENDS ${GENERATED_PROTOCOL_DIR}/xdg-shell-protocol.h
)

add_executable(wlcomp
    src/main.cpp
)

pkg_check_modules(WAYLAND_SERVER REQUIRED wayland-server IMPORTED_TARGET)

target_link_libraries(wlcomp PRIVATE PkgConfig::WLROOTS PkgConfig::WAYLAND_SERVER)
target_include_directories(wlcomp PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GENERATED_PROTOCOL_DIR}
)
add_dependencies(wlcomp generate-protocol-headers)
