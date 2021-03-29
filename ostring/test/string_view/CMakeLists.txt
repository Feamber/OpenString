
add_executable(open_string_view_test
	test_string_view.cpp
)
target_link_libraries(open_string_view_test
	gtest_main
	open_string
)

include(GoogleTest)
gtest_discover_tests(open_string_view_test)