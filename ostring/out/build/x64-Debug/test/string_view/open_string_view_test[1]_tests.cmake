add_test( construct.literal D:/git/OpenString/OpenString/ostring/out/build/x64-Debug/test/string_view/open_string_view_test.exe [==[--gtest_filter=construct.literal]==] --gtest_also_run_disabled_tests)
set_tests_properties( construct.literal PROPERTIES WORKING_DIRECTORY D:/git/OpenString/OpenString/ostring/out/build/x64-Debug/test/string_view SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( open_string_view_test_TESTS construct.literal)
