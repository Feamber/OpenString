project(ostring_test)
enable_testing() 
include(CTest)

file(GLOB_RECURSE sources *.cpp)
add_library(ostring_test_source ${sources})
link_libraries(ostring_test_source)

FILE(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/*)
FOREACH(child ${children})
IF(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${child})
ENDIF()
ENDFOREACH()
