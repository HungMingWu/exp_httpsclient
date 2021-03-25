project(http_client_async)
add_executable(${PROJECT_NAME} http_client_async.cpp)
target_link_libraries(${PROJECT_NAME}
PRIVATE
	Threads::Threads
	beast
)
