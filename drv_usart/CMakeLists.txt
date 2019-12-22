cmake_minimum_required(VERSION 3.13.1)

list(APPEND APP_SOURCES
    test_cases.c
)

include(../../mcuenv/cmake/boilerplate_unit_stm32drv.cmake)

project(app)
