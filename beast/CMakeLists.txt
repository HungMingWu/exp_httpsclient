cmake_minimum_required (VERSION 3.15)

project(beast)
add_library(beast INTERFACE)
target_include_directories(beast
INTERFACE
	./
	./asio/asio/include
)

target_compile_features(beast 
INTERFACE 
	cxx_std_17
)