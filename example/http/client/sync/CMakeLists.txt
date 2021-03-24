project(http_client_sync)
add_executable(${PROJECT_NAME} http_client_sync.cpp)
target_link_libraries(${PROJECT_NAME}
PRIVATE
	Threads::Threads
	beast
)