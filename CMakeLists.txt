cmake_minimum_required (VERSION 3.15)

project(demo)
find_package(Threads REQUIRED)
find_package(OpenSSL)
add_subdirectory(tests)
add_subdirectory(beast)
add_subdirectory(example)