project(websocket_client_async_ssl_system_executor)
add_executable(${PROJECT_NAME} websocket_client_async_ssl_system_executor.cpp)
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
IF (MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
ENDIF()