project(http_client_async_ssl)
add_executable(${PROJECT_NAME} http_client_async_ssl.cpp)
target_include_directories(${PROJECT_NAME}
PRIVATE
	../../../common
)
target_link_libraries(${PROJECT_NAME}
PRIVATE
	Threads::Threads
	OpenSSL::SSL OpenSSL::Crypto
	beast
)
