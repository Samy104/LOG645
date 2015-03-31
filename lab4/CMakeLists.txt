cmake_minimum_required(VERSION 3.1)
project(lab4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    main.cpp
    seq.c)

add_executable(lab4 ${SOURCE_FILES} main.cpp main.h)