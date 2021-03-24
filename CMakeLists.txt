cmake_minimum_required (VERSION 3.15)

project(demo)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
add_executable (demo http_client_sync_ssl.cpp)
target_include_directories(demo
PRIVATE
	./	
)

target_link_libraries(demo
PRIVATE
	Threads::Threads
	OpenSSL::SSL OpenSSL::Crypto
)
