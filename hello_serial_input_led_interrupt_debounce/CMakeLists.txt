cmake_minimum_required(VERSION 3.22)
set(BOARD esp32)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project (hello_world_blinky)

target_sources(app PRIVATE src/main.c)
