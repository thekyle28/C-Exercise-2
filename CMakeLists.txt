cmake_minimum_required(VERSION 3.3)
project(C_Exercise_2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    evalexp.c evalexpmain.c evalexp.h)

add_executable(C_Exercise_2 ${SOURCE_FILES})