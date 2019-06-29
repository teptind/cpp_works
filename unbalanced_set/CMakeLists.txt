cmake_minimum_required(VERSION 3.13)
project(ElderSet)

set(CMAKE_CXX_STANDARD 14)

add_executable(ElderSet
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc set_test.cpp
        counted.h
        counted.cpp
        fault_injection.h
        fault_injection.cpp
        my_set.h)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(ElderSet -lpthread)