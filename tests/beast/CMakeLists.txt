add_executable(tests main.cpp "core/basic_stream.cpp" "core/buffer_traits.cpp")
target_link_libraries(tests
PRIVATE
	beast
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