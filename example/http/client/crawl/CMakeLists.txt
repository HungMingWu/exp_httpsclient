project(http_crawl)
add_executable(${PROJECT_NAME} http_crawl.cpp urls_large_data.cpp)
target_link_libraries(${PROJECT_NAME}
PRIVATE
	Threads::Threads
	beast
)
