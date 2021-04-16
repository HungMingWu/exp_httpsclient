add_executable(tests main.cpp)
find_package(Threads REQUIRED)
target_link_libraries(tests
PRIVATE
	beast
	Threads::Threads
)

target_include_directories(tests
PRIVATE
	inc/
)

target_compile_definitions(tests
PRIVATE
	BOOST_BEAST_TESTS=1
)

add_subdirectory(core)
