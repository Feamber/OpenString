
#include <gtest/gtest.h>

#include "ostring/osv.h"
#include "ostring/ostr.h"

namespace osv {
	TEST(osv, length)
	{
		using namespace ostr;
		using namespace ostr::literal;

		EXPECT_EQ(2, u"an"_o.length());
		EXPECT_EQ(2, u"an"_o.origin_length());
		EXPECT_FALSE(u"an"_o.is_empty());
		EXPECT_EQ(0, u""_o.length());
		EXPECT_TRUE(u""_o.is_empty());
		EXPECT_EQ(7, u"我😘😘ni"_o.origin_length());
		EXPECT_EQ(5, u"我😘😘ni"_o.length());
		EXPECT_FALSE(u"我😘😘ni"_o.is_empty());
	}

	TEST(osv, literal) {
		using namespace ostr;
		using namespace ostr::literal;

		string_view sv_inconst = u"a"_o;
		EXPECT_EQ(1, sv_inconst.length());

		constexpr string_view sv_constexpr = u"bb"_o;
		EXPECT_EQ(2, sv_constexpr.length());

		sv_inconst = u"你😀"_o;
		EXPECT_EQ(3, sv_inconst.origin_length());
		EXPECT_EQ(2, sv_inconst.length());

		sv_inconst = sv_constexpr;
		EXPECT_EQ(2, sv_inconst.length());
	}

	TEST(osv, equal)
	{
		using namespace ostr;
		using namespace ostr::literal;

		string_view sv_an_1 = u"an"_o;
		string_view sv_an_2 = u"an"_o;
		EXPECT_TRUE(sv_an_1 == sv_an_2);

		string_view sv_ansi_aa = u"aa"_o;
		EXPECT_FALSE(sv_an_1 == sv_ansi_aa);
		EXPECT_FALSE(sv_an_2 == sv_ansi_aa);


		EXPECT_TRUE(sv_ansi_aa < sv_an_1);
	}

	TEST(osv, compare)
	{
		using namespace ostr;
		using namespace ostr::literal;

		string_view sv_an_1 = u"an"_o;
		string_view sv_an_2 = u"an"_o;

		string_view sv_ansi_aa = u"aa"_o;

		EXPECT_EQ(0, sv_an_1.compare(sv_an_1));
		EXPECT_EQ(0, sv_an_1.compare(sv_an_2));

		EXPECT_EQ(-1, sv_ansi_aa.compare(sv_an_1));
		EXPECT_EQ(1, sv_an_1.compare(sv_ansi_aa));
	}

	TEST(osv, index_of)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv_src1(u"1231234"_o);
			string_view sv_to_find1(u"123"_o);
			string_view sv_to_find2(u"1234"_o);

			EXPECT_EQ(0, sv_src1.index_of(sv_to_find1));
			EXPECT_EQ(3, sv_src1.index_of(sv_to_find2));
		}

		{
			string_view sv_src1(u"abcabcd"_o);
			string_view sv_to_find1(u"Abc"_o);
			string_view sv_to_find2(u"ABcD"_o);

			EXPECT_EQ(SIZE_MAX, sv_src1.index_of(sv_to_find1));
			EXPECT_EQ(SIZE_MAX, sv_src1.index_of(sv_to_find2));
			EXPECT_EQ(0, sv_src1.index_of(sv_to_find1, case_sensitivity::insensitive));
			EXPECT_EQ(3, sv_src1.index_of(sv_to_find2, case_sensitivity::insensitive));
		}

		{
			string_view sv_src1(u"我😘😘ni"_o);
			string_view sv_to_find1(u"😘"_o);
			string_view sv_to_find2(u"😘n"_o);
			string_view sv_to_find3(u"n"_o);

			EXPECT_EQ(1, sv_src1.index_of(sv_to_find1));
			EXPECT_EQ(2, sv_src1.index_of(sv_to_find2));
			EXPECT_EQ(3, sv_src1.index_of(sv_to_find3));
		}
	}

	TEST(osv, last_index_of)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv = u"abcdefghi"_o;
			{
				size_t i = sv.last_index_of(u"ghi"_o);
				EXPECT_EQ(6, i);
			}
			{
				size_t i = sv.last_index_of(u"fg"_o);
				EXPECT_EQ(5, i);
			}
			{
				size_t i = sv.last_index_of(u"jk"_o);
				EXPECT_EQ(SIZE_MAX, i);
			}
		}
		{
			string_view sv = u"😘™😘我™我"_o;
			{
				size_t i = sv.last_index_of(u"™"_o);
				EXPECT_EQ(4, i);
			}
			{
				size_t i = sv.last_index_of(u"😘我"_o);
				EXPECT_EQ(2, i);
			}
			{
				size_t i = sv.last_index_of(u"我"_o);
				EXPECT_EQ(5, i);
			}
		}
	}

	TEST(osv, search)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			size_t s = u"abcdefghi"_o.search([](char c) {return c == u'c'; });
			EXPECT_EQ(2, s);
		}
		{
			size_t s1 = u"😘™😘我™我"_o.search([](char16_t c) {return c == u'我'; });
			EXPECT_EQ(3, s1);
			size_t s2 = u"😘™😘我™我"_o.search([](char32_t c) {return c == U'😘'; });
			EXPECT_EQ(0, s2);
		}
	}

	TEST(osv, split)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			std::vector<string_view> parts;
			size_t split_time = u""_o.split(u"111"_o, parts);
			EXPECT_EQ(0, split_time);
			EXPECT_EQ(1, parts.size());
			EXPECT_TRUE(parts[0] == u""_o);
		}
		{
			std::vector<string_view> parts;
			size_t split_time = u"eveea"_o.split(u"e"_o, parts);
			EXPECT_EQ(3, split_time);
			EXPECT_EQ(4, parts.size());
			EXPECT_TRUE(parts[0] == u""_o);
			EXPECT_TRUE(parts[1] == u"v"_o);
			EXPECT_TRUE(parts[2] == u""_o);
			EXPECT_TRUE(parts[3] == u"a"_o);
		}
		{
			std::vector<string_view> parts;
			size_t split_time = u"eveea"_o.split(u"e"_o, parts, true);
			EXPECT_EQ(3, split_time);
			EXPECT_EQ(2, parts.size());
			EXPECT_TRUE(parts[0] == u"v"_o);
			EXPECT_TRUE(parts[1] == u"a"_o);
		}
		{
			std::vector<string_view> parts;
			size_t split_time = u"我，真，是，太，™，菜，了"_o.split(u"，"_o, parts);
			EXPECT_EQ(6, split_time);
			EXPECT_EQ(7, parts.size());
			EXPECT_TRUE(parts[0] == u"我"_o);
			EXPECT_TRUE(parts[1] == u"真"_o);
			EXPECT_TRUE(parts[2] == u"是"_o);
			EXPECT_TRUE(parts[3] == u"太"_o);
			EXPECT_TRUE(parts[4] == u"™"_o);
			EXPECT_TRUE(parts[5] == u"菜"_o);
			EXPECT_TRUE(parts[6] == u"了"_o);
		}
	}

	TEST(osv, start_with)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv = u"abcdefghi"_o;
			EXPECT_TRUE(sv.start_with(u"abcd"_o));
			EXPECT_FALSE(sv.start_with(u"b"_o));
		}
		{
			string_view sv = u"😘™😘我™我"_o;
			EXPECT_TRUE(sv.start_with(u"😘"_o));
			EXPECT_FALSE(sv.start_with(u"™"_o));
		}
	}

	TEST(osv, end_with)
	{
		using namespace ostr;
		using namespace ostr::literal;
		{
			string_view sv = u"abcdefghi"_o;
			EXPECT_TRUE(sv.end_with(u"ghi"_o));
			EXPECT_FALSE(sv.end_with(u"h"_o));
		}
		{
			string_view sv = u"😘™😘我™我"_o;
			EXPECT_TRUE(sv.end_with(u"我"_o));
			EXPECT_FALSE(sv.end_with(u"™"_o));
		}
	}

	TEST(osv, remove_xfix)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv_wide_an = u"anti-aliasing"_o;
			EXPECT_TRUE(sv_wide_an.remove_prefix(5) == u"aliasing"_o);
			EXPECT_TRUE(sv_wide_an.remove_suffix(3) == u"anti-alias"_o);
		}
		{
			string_view sv_wide_an = u"我😘😘ni"_o;
			EXPECT_TRUE(sv_wide_an.remove_prefix(2) == u"😘ni"_o);
			EXPECT_TRUE(sv_wide_an.remove_suffix(3) == u"我😘"_o);
		}
	}

	TEST(osv, substring)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv_wide_an = u"anti-aliasing"_o;
			EXPECT_TRUE(sv_wide_an.substring(2) == u"ti-aliasing"_o);
		}
		{
			string_view sv_wide_an = u"anti-aliasing"_o;
			EXPECT_TRUE(sv_wide_an.substring(2, 5) == u"ti-al"_o);
		}
		{
			string_view sv_wide_an = u"anti-aliasing"_o;
			EXPECT_TRUE(sv_wide_an.substring(2, 0) == u""_o);
		}
	}

	TEST(osv, left_right)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv_wide_an = u"anti-aliasing"_o;
			EXPECT_TRUE(sv_wide_an.left(5) == u"anti-"_o);
			EXPECT_TRUE(sv_wide_an.right(3) == u"ing"_o);
		}
		{
			string_view sv_wide_an = u"我😘😘ni"_o;
			EXPECT_TRUE(sv_wide_an.left(2) == u"我😘"_o);
			EXPECT_TRUE(sv_wide_an.right(3) == u"😘ni"_o);
		}
	}

	TEST(osv, trim)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv = u"   an ti-alias ing    "_o;
			EXPECT_TRUE(sv.trim_start() == u"an ti-alias ing    "_o);
			EXPECT_TRUE(sv.trim_end() == u"   an ti-alias ing"_o);
			EXPECT_TRUE(sv.trim() == u"an ti-alias ing"_o);
		}

		{
			string_view sv = u"an ti-alias ing"_o;
			EXPECT_TRUE(sv.trim_start() == u"an ti-alias ing"_o);
			EXPECT_TRUE(sv.trim_end() == u"an ti-alias ing"_o);
			EXPECT_TRUE(sv.trim() == u"an ti-alias ing"_o);
		}

		{
			string_view sv = u"   你 好 😙    "_o;
			EXPECT_TRUE(sv.trim_start() == u"你 好 😙    "_o);
			EXPECT_TRUE(sv.trim_end() == u"   你 好 😙"_o);
			EXPECT_TRUE(sv.trim() == u"你 好 😙"_o);
		}

		{
			string_view sv = u"你 好 😙"_o;
			EXPECT_TRUE(sv.trim_start() == u"你 好 😙"_o);
			EXPECT_TRUE(sv.trim_end() == u"你 好 😙"_o);
			EXPECT_TRUE(sv.trim() == u"你 好 😙"_o);
		}

		{
			string_view sv = u"    "_o;
			EXPECT_TRUE(sv.trim_start() == u""_o);
			EXPECT_TRUE(sv.trim_end() == u""_o);
			EXPECT_TRUE(sv.trim() == u""_o);
		}
	}

	TEST(osv, to_string)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv = u"123"_o.substring(0, 1);
			string s = sv;
			EXPECT_TRUE(s == u"1"_o);
		}
		{
			string_view sv = u"我😘😘ni"_o.substring(1, 1);
			string s = sv;
			EXPECT_TRUE(s == u"😘"_o);
		}
	}

	TEST(osv, iterator)
	{
		using namespace ostr;
		using namespace ostr::literal;

		{
			string_view sv = u"123456"_o;
			char32_t ec = u'1';
			for (auto c : sv)
			{
				EXPECT_TRUE(c == ec);
				++ec;
			}
		}
		{
			string_view sv = u"你好我是中文"_o;
			size_t ind = 0;
			for (auto c : sv)
			{
				EXPECT_TRUE(c == u"你好我是中文"[ind]);
				++ind;
			}
		}
		{
			string_view sv = u"😘好 ™𪚥😁"_o;
			size_t ind = 0;
			for (auto c : sv)
			{
				EXPECT_TRUE(c == U"😘好 ™𪚥😁"[ind]);
				++ind;
			}
		}
		{
			string_view sv = u"😘𪚥😁"_o;
			size_t ind = 0;
			for (auto it = sv.cbegin(); it != sv.cend(); ++it)
			{
				EXPECT_TRUE(*it == U"😘𪚥😁"[ind]);
				EXPECT_TRUE(it.get_origin_index() == ind * 2);
				EXPECT_TRUE(it.get_origin_length() == 2);
				++ind;
			}
		}
	}

	TEST(osv, hash)
	{
		using namespace ostr;
		using namespace ostr::literal;
		EXPECT_EQ(u""_o.get_hash(), 0);
		EXPECT_EQ(u"stack-overflow"_o.get_hash(), 0x335CC04A);
		EXPECT_EQ(sv_hasher{}(u"stack-overflow"_o), 0x335CC04A);
	}

}