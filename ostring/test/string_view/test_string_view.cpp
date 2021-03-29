
#include <gtest/gtest.h>

#include "ostring/osv.h"

namespace osv
{
	TEST(construct, literal) {
		using namespace ostr;
		using namespace ostr::literal;

		string_view sv_inconst = OSTR("a");
		EXPECT_TRUE(sv_inconst.length() == 1);

		constexpr string_view sv_constexpr = u"bb"o;
		EXPECT_TRUE(sv_constexpr.length() == 2);

		sv_inconst = OSTR("ccc");
		EXPECT_TRUE(sv_inconst.length() == 3);

		sv_inconst = sv_constexpr;
		EXPECT_TRUE(sv_inconst.length() == 2);
	}
}

/*#include "pch.h"
#include "CppUnitTest.h"
#include "../ostring/string.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_string_view
{
	TEST_CLASS(test_construct)
	{
	public:

		TEST_METHOD(sv_construct_ansi)
		{
			using namespace ostr;

			string_view sv_inconst = OSTR("a");
			EXPECT_TRUE(sv_inconst.length() == 1);

			constexpr string_view sv_constexpr = OSTR("bb");
			EXPECT_TRUE(sv_constexpr.length() == 2);

			sv_inconst = OSTR("ccc");
			EXPECT_TRUE(sv_inconst.length() == 3);

			sv_inconst = sv_constexpr;
			EXPECT_TRUE(sv_inconst.length() == 2);
		}

		TEST_METHOD(sv_construct_wide)
		{
			using namespace ostr;

			string_view sv_inconst = OSTR("a");
			EXPECT_TRUE(sv_inconst.length() == 1);

			constexpr string_view sv_constexpr = OSTR("bb");
			EXPECT_TRUE(sv_constexpr.length() == 2);

			sv_inconst = OSTR("ccc");
			EXPECT_TRUE(sv_inconst.length() == 3);

			sv_inconst = sv_constexpr;
			EXPECT_TRUE(sv_inconst.length() == 2);
		}
		TEST_METHOD(sv_compare)
		{
			using namespace ostr;

			string_view sv_wide_an = OSTR("an");
			string_view sv_ansi_an = OSTR("an");
			EXPECT_TRUE(sv_wide_an == sv_ansi_an);

			string_view sv_ansi_aa = OSTR("aa");
			Assert::IsFalse(sv_ansi_an == sv_ansi_aa);
			Assert::IsFalse(sv_wide_an == sv_ansi_aa);


			EXPECT_TRUE(sv_ansi_aa < sv_ansi_an);
		}
		TEST_METHOD(sv_length)
		{
			using namespace ostr;

			Assert::AreEqual<size_t>(2, OSTR("an").length());
			Assert::AreEqual<size_t>(0, OSTR("").length());
			Assert::AreEqual<size_t>(7, OSTR("我😘😘ni").origin_length());
			Assert::AreEqual<size_t>(5, OSTR("我😘😘ni").length());
		}
		TEST_METHOD(sv_index_of)
		{
			using namespace ostr;

			{
				string_view sv_src1(OSTR("1231234"));
				string_view sv_to_find1(OSTR("123"));
				string_view sv_to_find2(OSTR("1234"));

				Assert::AreEqual<size_t>(0, sv_src1.index_of(sv_to_find1));
				Assert::AreEqual<size_t>(3, sv_src1.index_of(sv_to_find2));
			}

			{
				string_view sv_src1(OSTR("abcabcd"));
				string_view sv_to_find1(OSTR("Abc"));
				string_view sv_to_find2(OSTR("ABcD"));

				Assert::AreEqual<size_t>(SIZE_MAX, sv_src1.index_of(sv_to_find1));
				Assert::AreEqual<size_t>(SIZE_MAX, sv_src1.index_of(sv_to_find2));
				Assert::AreEqual<size_t>(0, sv_src1.index_of(sv_to_find1, case_sensitivity::insensitive));
				Assert::AreEqual<size_t>(3, sv_src1.index_of(sv_to_find2, case_sensitivity::insensitive));
			}

			{
				string_view sv_src1(OSTR("我😘😘ni"));
				string_view sv_to_find1(OSTR("😘"));
				string_view sv_to_find2(OSTR("😘n"));
				string_view sv_to_find3(OSTR("n"));

				Assert::AreEqual<size_t>(1, sv_src1.index_of(sv_to_find1));
				Assert::AreEqual<size_t>(2, sv_src1.index_of(sv_to_find2));
				Assert::AreEqual<size_t>(3, sv_src1.index_of(sv_to_find3));
			}
		}
		TEST_METHOD(sv_last_index_of)
		{
			using namespace ostr;

			{
				string_view sv = OSTR("abcdefghi");
				{
					size_t i = sv.last_index_of(OSTR("ghi"));
					Assert::AreEqual<size_t>(6, i); 
				}
				{
					size_t i = sv.last_index_of(OSTR("fg"));
					Assert::AreEqual<size_t>(5, i); 
				}
				{
					size_t i = sv.last_index_of(OSTR("jk"));
					Assert::AreEqual<size_t>(SIZE_MAX, i); 
				}
			}
			{
				string_view sv = OSTR("😘™😘我™我");
				{
					size_t i = sv.last_index_of(OSTR("™"));
					Assert::AreEqual<size_t>(4, i);
				}
				{
					size_t i = sv.last_index_of(OSTR("😘"));
					Assert::AreEqual<size_t>(3, i);
				}
				{
					size_t i = sv.last_index_of(OSTR("我"));
					Assert::AreEqual<size_t>(5, i);
				}
			}
		}
		TEST_METHOD(sv_start_ends_with)
		{
			using namespace ostr;

			{
				string_view sv = OSTR("abcdefghi");
				EXPECT_TRUE(sv.start_with(OSTR("abcd")));
				EXPECT_TRUE(sv.ends_with(OSTR("ghi")));
				Assert::IsFalse(sv.start_with(OSTR("b")));
				Assert::IsFalse(sv.ends_with(OSTR("h")));
			}
			{
				string_view sv = OSTR("abcdefghi");
				EXPECT_TRUE(sv.start_with(OSTR("abcd")));
				EXPECT_TRUE(sv.ends_with(OSTR("ghi")));
				Assert::IsFalse(sv.start_with(OSTR("b")));
				Assert::IsFalse(sv.ends_with(OSTR("h")));
			}
		}
		TEST_METHOD(sv_remove_prefix_suffix)
		{
			using namespace ostr;

			{
				string_view sv_wide_an = OSTR("anti-aliasing");
				sv_wide_an.remove_prefix(5);
				EXPECT_TRUE(sv_wide_an == OSTR("aliasing"));

				sv_wide_an.remove_suffix(3);
				EXPECT_TRUE(sv_wide_an == OSTR("alias")); 
			}
			{
				string_view sv_wide_an = OSTR("anti-aliasing");
				EXPECT_TRUE(sv_wide_an.remove_prefix_copy(5) == OSTR("aliasing"));

				EXPECT_TRUE(sv_wide_an.remove_suffix_copy(3) == OSTR("anti-alias"));
			}
			{
				string_view sv_wide_an = OSTR("我😘😘ni");
				EXPECT_TRUE(sv_wide_an.remove_prefix_copy(2) == OSTR("😘ni"));
				EXPECT_TRUE(sv_wide_an.remove_suffix_copy(3) == OSTR("我😘"));
			}
		}
		TEST_METHOD(sv_substring)
		{
			using namespace ostr;

			{
				string_view sv_wide_an = OSTR("anti-aliasing");
				EXPECT_TRUE(sv_wide_an.substring(2) == OSTR("ti-aliasing"));
			}
			{
				string_view sv_wide_an = OSTR("anti-aliasing");
				EXPECT_TRUE(sv_wide_an.substring(2, 5) == OSTR("ti-al"));
			}
			{
				string_view sv_wide_an = OSTR("anti-aliasing");
				EXPECT_TRUE(sv_wide_an.substring(2, 0) == OSTR(""));
			}
		}
		TEST_METHOD(sv_to_string)
		{
			using namespace ostr;
			
			{
				string_view sv = OSTR("123").substring(0, 1);
				string s = sv;
				EXPECT_TRUE(s == "1"); 
			}
			{
				string_view sv = OSTR("我😘😘ni").substring(1, 1);
				string s = sv;
				EXPECT_TRUE(s == OSTR("😘"));
			}
		}
		TEST_METHOD(sv_split)
		{
			using namespace ostr;

			{
				std::vector<string_view> parts;
				size_t split_time = OSTR("").split(OSTR("111"), parts);
				Assert::AreEqual<size_t>(0, split_time);
				Assert::AreEqual<size_t>(1, parts.size());
				EXPECT_TRUE(parts[0] == OSTR(""));
			}
			{
				std::vector<string_view> parts;
				size_t split_time = OSTR("eveea").split(OSTR("e"), parts);
				Assert::AreEqual<size_t>(3, split_time);
				Assert::AreEqual<size_t>(4, parts.size());
				EXPECT_TRUE(parts[0] == OSTR(""));
				EXPECT_TRUE(parts[1] == OSTR("v"));
				EXPECT_TRUE(parts[2] == OSTR(""));
				EXPECT_TRUE(parts[3] == OSTR("a"));
			}
			{
				std::vector<string_view> parts;
				size_t split_time = OSTR("eveea").split(OSTR("e"), parts, true);
				Assert::AreEqual<size_t>(3, split_time);
				Assert::AreEqual<size_t>(2, parts.size());
				EXPECT_TRUE(parts[0] == OSTR("v"));
				EXPECT_TRUE(parts[1] == OSTR("a"));
			}
			{
				std::vector<string_view> parts;
				size_t split_time = OSTR("我，真，是，太，™，菜，了").split(OSTR("，"), parts);
				Assert::AreEqual<size_t>(6, split_time);
				Assert::AreEqual<size_t>(7, parts.size());
				EXPECT_TRUE(parts[0] == OSTR("我"));
				EXPECT_TRUE(parts[1] == OSTR("真"));
				EXPECT_TRUE(parts[2] == OSTR("是"));
				EXPECT_TRUE(parts[3] == OSTR("太"));
				EXPECT_TRUE(parts[4] == OSTR("™"));
				EXPECT_TRUE(parts[5] == OSTR("菜"));
				EXPECT_TRUE(parts[6] == OSTR("了"));
			}
		}
	};
}*/