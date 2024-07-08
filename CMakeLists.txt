cmake_minimum_required(VERSION 3.13)

# SDK setup stuff
include(../pico-sdk/pico-sdk/pico_sdk_init.cmake)
set(PROJECT project)
project(${PROJECT})
pico_sdk_init()

# Executable stuff
add_executable(${PROJECT} src/main.cpp src/u8g2_port.cpp)
add_compile_options(-Wall -Wextra -fdata-sections -ffunction-sections)

# U8G2
add_subdirectory(lib/u8g2)
target_include_directories(${PROJECT} PRIVATE lib/u8g2/cppsrc)
target_link_libraries(${PROJECT} pico_stdlib hardware_i2c u8g2)
target_link_options(${PROJECT} PRIVATE -Wl,--gc-sections -Wl,--print-memory-usage)

# IDK but it's necessary
pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)
pico_add_extra_outputs(${PROJECT})

