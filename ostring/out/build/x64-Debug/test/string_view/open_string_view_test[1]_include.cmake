if(EXISTS "D:/git/OpenString/OpenString/ostring/out/build/x64-Debug/test/string_view/open_string_view_test[1]_tests.cmake")
  include("D:/git/OpenString/OpenString/ostring/out/build/x64-Debug/test/string_view/open_string_view_test[1]_tests.cmake")
else()
  add_test(open_string_view_test_NOT_BUILT open_string_view_test_NOT_BUILT)
endif()
