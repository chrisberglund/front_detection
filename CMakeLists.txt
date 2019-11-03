cmake_minimum_required(VERSION 3.15)
project(sied C)

set(CMAKE_C_STANDARD 99)

add_library(sied SHARED histogram.c histogram.h helpers.c helpers.h cohesion.c cohesion.h locator.c locator.h contour.c contour.h cayula.c cayula.h prefilter.c prefilter.h filter.c filter.h)