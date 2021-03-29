add_executable(tests main.cpp)
target_link_libraries(tests
PRIVATE
	beast
)

target_include_directories(tests
PRIVATE
	catch2/
)

target_compile_definitions(tests
PRIVATE
	BOOST_BEAST_TESTS=1
)

add_subdirectory(core)