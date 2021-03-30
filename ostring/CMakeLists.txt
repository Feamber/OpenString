cmake_minimum_required (VERSION 3.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

option(OPEN_STRING_TESTS "Build Test Targets." ON)
option(OPEN_STRING_SAMPLE "Execute Main" ON)

project(open_string)

add_subdirectory(extern)

file(GLOB_RECURSE headers include/*.h)
file(GLOB_RECURSE sources source/*.cpp)

#add_library(open_string ${headers} ${sources})
add_library(open_string ${headers} ${sources})

target_include_directories(open_string 
    PUBLIC 
    include/
    ${PLATFORM_INCLUDES}
)

target_link_libraries(open_string PUBLIC fmt)
target_link_libraries(open_string PUBLIC spdlog)

if(OPEN_STRING_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(OPEN_STRING_TESTS)

if(OPEN_STRING_SAMPLE)
    add_subdirectory(sample)
endif(OPEN_STRING_SAMPLE)
