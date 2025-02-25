cmake_minimum_required(VERSION 3.16)

project(cpp-demo)

find_package(cifuzz NO_SYSTEM_ENVIRONMENT_PATH)
enable_fuzz_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

add_fuzz_test(fuzztest fuzztest.cpp)
target_link_libraries(fuzztest automotive)
