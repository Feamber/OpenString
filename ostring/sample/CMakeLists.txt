set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB sources *.cpp)
file(GLOB_RECURSE headers *.h)

add_executable(open_string_sample ${sources} ${platform_sources} ${headers})

target_include_directories(open_string_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")
target_link_libraries(open_string_sample open_string)

set_property(TARGET open_string_sample PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/sample")