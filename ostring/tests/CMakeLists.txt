cmake_minimum_required(VERSION 3.18)
project(open_string_tests)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(open_string_tests
	"test_main.cpp"
	"helper_test.cpp"
	"string_test.cpp"
	"string_view_test.cpp"
	
)
target_link_libraries(open_string_tests
	gtest_main
	open_string
)

target_include_directories(open_string_tests 
    PUBLIC 
    ostring/include/
    ${PLATFORM_INCLUDES}
)

include(GoogleTest)
gtest_discover_tests(open_string_tests)