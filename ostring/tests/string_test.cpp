
#include <gtest/gtest.h>

#include "ostring/ostr.h"

TEST(ostr, literal)
{
	using namespace ostr;
	using namespace ostr::literal;

	const char* str_cp = "this is a const char pointer";
	std::string str_std = "this is a std::string";
	std::string_view str_stdsv = "this is a std::string_view";

	string str_from_cp(str_cp);
	string str_from_std(str_std);
	string str_from_stdsv(str_stdsv);
	string str_default_ctor;
	string str_empty_ansi("");
	string str_empty_wide(u""_o);
	string str_ctor_ansi("123321");
	string str_assign = "123321";
	string str_ctor_wide(u"123321"_o);
	string str_ctor_ansi_another("abccd");
	string str_ctor_hans(u"我"_o);
	string str_ctor_hans_another(u"我他"_o);
	string str_ctor_supplement(u"𪚥😁"_o);
	string str_ctor_supplement_2(u"𪚥😁"_o);
	string str_ctor_supplement_3(u"𪚥"_o);
	string str_ctor_6c_ansi('c', 6);
	string str_ctor_6c_wide(L'c', 6);
	string str_ctor_6c_string("cccccc");
	string str_ctor_substring1(u"123321", 3);
	string str_ctor_substring2(u"123"_o);

	EXPECT_TRUE(str_from_cp == str_cp);
	EXPECT_TRUE(str_from_std == str_std);
	EXPECT_TRUE(str_from_stdsv == str_stdsv);
	EXPECT_TRUE(str_default_ctor == str_default_ctor);
	EXPECT_TRUE(str_default_ctor == str_empty_ansi);
	EXPECT_TRUE(str_empty_ansi == str_empty_wide);
	EXPECT_TRUE(str_default_ctor == str_empty_wide);
	EXPECT_TRUE(str_ctor_ansi == str_ctor_ansi);
	EXPECT_TRUE(str_ctor_ansi == str_assign);
	EXPECT_TRUE(str_assign == str_ctor_wide);
	EXPECT_TRUE(str_ctor_ansi == str_ctor_wide);
	EXPECT_TRUE(str_ctor_wide == str_ctor_wide);
	EXPECT_FALSE(str_ctor_ansi == str_ctor_ansi_another);
	EXPECT_FALSE(str_ctor_hans == str_ctor_hans_another);
	EXPECT_TRUE(str_ctor_supplement == str_ctor_supplement_2);
	EXPECT_FALSE(str_ctor_supplement == str_ctor_supplement_3);
	EXPECT_TRUE(str_ctor_6c_ansi == str_ctor_6c_string);
	EXPECT_TRUE(str_ctor_6c_wide == str_ctor_6c_string);
	EXPECT_TRUE(str_ctor_substring1 == str_ctor_substring2);
}

TEST(ostr, compare)
{
	using namespace ostr;
	using namespace ostr::literal;

	string str_default_ctor;
	string str_empty_ansi("");
	string str_empty_wide(u""_o);
	string str_ctor_ansi("123321");
	string str_assign = "123321";
	string str_ctor_wide(u"123321"_o);
	string str_ctor_ansi_another("abccd");
	string str_ctor_hans(u"我"_o);
	string str_ctor_hans_another(u"我他"_o);
	string str_ctor_supplement(u"𪚥😁"_o);
	string str_ctor_supplement_2(u"𪚥😁"_o);
	string str_ctor_supplement_3(u"𪚥"_o);

	EXPECT_EQ(0, str_default_ctor.compare(str_empty_ansi));
	EXPECT_EQ(-1, str_empty_wide.compare(str_assign));
	EXPECT_EQ(0, str_assign.compare(str_ctor_wide));
	EXPECT_EQ(0, str_ctor_wide.compare(str_assign));
	EXPECT_EQ(-1, str_ctor_wide.compare(str_ctor_ansi_another));
	EXPECT_EQ(-1, str_ctor_hans.compare(str_ctor_hans_another));
	EXPECT_EQ(0, str_ctor_supplement.compare(str_ctor_supplement_2));
	EXPECT_EQ(1, str_ctor_supplement.compare(str_ctor_supplement_3));
}
TEST(ostr, length)
{
	using namespace ostr;
	using namespace ostr::literal;

	string str_default_ctor;
	string str_empty_ansi("");
	string str_empty_wide(u""_o);
	string str_ctor_ansi("123321");
	string str_assign = "123321";
	string str_ctor_wide(u"123321"_o);
	string str_ctor_ansi_another("abccd");
	string str_ctor_hans(u"我"_o);
	string str_ctor_hans_another(u"我他"_o);
	string str_ctor_supplement(u"𪚥😁"_o);
	string str_ctor_supplement_2(u"𪚥"_o);

	EXPECT_EQ(0, str_default_ctor.length());
	EXPECT_TRUE(str_default_ctor.is_empty());
	EXPECT_EQ(0, str_empty_ansi.length());
	EXPECT_EQ(0, str_empty_wide.length());
	EXPECT_EQ(6, str_ctor_ansi.length());
	EXPECT_FALSE(str_ctor_ansi.is_empty());
	EXPECT_EQ(6, str_assign.length());
	EXPECT_EQ(6, str_ctor_wide.length());
	EXPECT_EQ(5, str_ctor_ansi_another.length());
	EXPECT_EQ(1, str_ctor_hans.length());
	EXPECT_EQ(2, str_ctor_hans_another.length());
	EXPECT_EQ(2, str_ctor_supplement.length());
	EXPECT_EQ(1, str_ctor_supplement_2.length());
}

TEST(ostr, combination)
{
	using namespace ostr;

	{
		const char char_wo[] = { 0xe6i8, 0x88i8, 0x91i8, 0x00i8 };
		string str_wo_utf8;
		str_wo_utf8.decode_from_utf8(char_wo);
		string str_ai(u"😘");
		string str_ni(u"ni");

		string combine = str_wo_utf8 + str_ai + str_ai + str_ni;
		string correct = u"我😘😘ni";

		EXPECT_TRUE(combine == correct);
	}
}

TEST(ostr, substring)
{
	using namespace ostr;
	using namespace ostr::literal;

	{
		string str_src1("123321");

		EXPECT_TRUE(str_src1.substring(3) == string("321"));
		EXPECT_TRUE(str_src1.substring(0, 4) == string("1233"));
		EXPECT_TRUE(str_src1.substring(1, 4) == string("2332"));

	}

	{
		string str_src2(u"233你abc"_o);

		EXPECT_TRUE(str_src2.substring(3) == string(u"你abc"));
		EXPECT_TRUE(str_src2.substring(0, 4) == string(u"233你"));
		EXPECT_TRUE(str_src2.substring(1, 4) == string(u"33你a"_o));
	}

	{
		string str_src3(u"我😘😘ni");

		EXPECT_TRUE(str_src3.substring(2) == string(u"😘ni"));
		EXPECT_TRUE(str_src3.substring(0, 4) == string(u"我😘😘n"_o));
		EXPECT_TRUE(str_src3.substring(1, 3) == string(u"😘😘n"));
	}
}

TEST(ostr, index_of)
{
	using namespace ostr;
	using namespace ostr::literal;

	{
		string str_src1("1231234");
		string str_to_find1("123");
		string str_to_find2(u"1234"_o);

		EXPECT_EQ(0, str_src1.index_of(str_to_find1));
		EXPECT_EQ(3, str_src1.index_of(str_to_find2));
		EXPECT_EQ(3, str_src1.index_of(str_to_find1, 2));
		EXPECT_EQ(3, str_src1.index_of(str_to_find1, 2, 5));
	}

	{
		string str_src1(u"我😘😘ni");
		string str_to_find1(u"😘"_o);
		string str_to_find2(u"😘n");
		string str_to_find3("n");

		EXPECT_EQ(1, str_src1.index_of(str_to_find1));
		EXPECT_EQ(3, str_src1.index_of(str_to_find3));
		EXPECT_EQ(2, str_src1.index_of(str_to_find1, 2));
		EXPECT_EQ(SIZE_MAX, str_src1.index_of(str_to_find2, 3, 2));
	}
}

TEST(ostr, last_index_of)
{
	using namespace ostr;
	using namespace ostr::literal;

	{
		string str_src1("1231234");
		string str_to_find1("123");
		string str_to_find2(u"1234"_o);

		EXPECT_EQ(3, str_src1.last_index_of(str_to_find1));
		EXPECT_EQ(3, str_src1.last_index_of(str_to_find2));
		EXPECT_EQ(3, str_src1.last_index_of(str_to_find1, 2));
		EXPECT_EQ(SIZE_MAX, str_src1.last_index_of(str_to_find1, 2, 3));
	}

	{
		string str_src1(u"我n😘😘ni");
		string str_to_find1(u"😘"_o);
		string str_to_find2(u"😘n");
		string str_to_find3("n");

		EXPECT_EQ(3, str_src1.last_index_of(str_to_find1));
		EXPECT_EQ(4, str_src1.last_index_of(str_to_find3));
		EXPECT_EQ(3, str_src1.last_index_of(str_to_find1, 3));
		EXPECT_EQ(SIZE_MAX, str_src1.last_index_of(str_to_find2, 2, 2));
	}
}

TEST(ostr, search)
{
	using namespace ostr;

	{
		string str("123456789");

		EXPECT_EQ(4, str.search([](char c) {return c > '4'; }));
		EXPECT_EQ(2, str.search([](wchar_t c) {return c > L'2'; }));
		EXPECT_EQ(8, str.search([](char16_t c) {return c > u'8'; }));
		EXPECT_EQ(4, str.search([](char32_t c) {return c == U'5'; }));
	}
	{
		string str(u"我😘れC♂");

		EXPECT_EQ(3, str.search([](char c) {return c == 'C'; }));
		EXPECT_EQ(4, str.search([](wchar_t c) {return c == L'♂'; }));
		EXPECT_EQ(2, str.search([](char16_t c) {return c == u'れ'; }));
		EXPECT_EQ(1, str.search([](char32_t c) {return c >= U'😘'; }));
	}
}

TEST(ostr, replace)
{
	using namespace ostr;
	using namespace ostr::literal;

	{
		string str("1231234");

		str.replace_origin(3, 2, u""_o);
		EXPECT_TRUE(str == "12334");
	}
	{
		string str("1231234");

		EXPECT_TRUE(str.replace_copy(u"123"_o, u"1234") == "123412344");
	}
	{
		string str(u"我♂😘Cれ");
		str.replace_origin(1, 2, u"™").replace_origin(2, 1, u"3"_o);

		EXPECT_TRUE(str == u"我™3れ"_o);
	}
	{
		string str(u"我😘れC♂");

		EXPECT_TRUE(str.replace_copy(u"我😘"_o, u"♂♂♂♂♂") == u"♂♂♂♂♂れC♂"_o);
	}
	{
		string str(u"我😘れC♂");

		EXPECT_TRUE(str.replace_copy(u"我れ"_o, u"33") == u"我😘れC♂"_o);
	}
}

TEST(ostr, format)
{
	using namespace ostr;
	using namespace ostr::literal;

	{
		string str_src1("123{}1234");

		string str = str_src1.format(u"a");
		EXPECT_TRUE(str == "123a1234");
	}
	{
		string str_src1("123{1}12{0}34");

		string str = str_src1.format(123, u"a");
		EXPECT_TRUE(str == "123a1212334");
	}
	{
		string str_src1("123{2}12{1}34");

		string str = str_src1.format(u"a"_o, 123, u"b");
		EXPECT_TRUE(str == "123b1212334");
	}
	{
		string str_src1(u"我{3}😘{0}れC{2}♂");

		string str = str_src1.format(u"a", u"o", u"™", u"™C𪚥");
		EXPECT_TRUE(str == u"我™C𪚥😘aれC™♂"_o);
		EXPECT_EQ(10, str.length());
	}
}

TEST(ostr, trim)
{
	using namespace ostr;
	using namespace ostr::literal;

	{ 
		{
			string str_src1("   123 1234    ");

			str_src1.trim_start();
			EXPECT_TRUE(str_src1 == "123 1234    ");
		}
		{
			string str_src1("   123 1234   ");

			str_src1.trim_end();
			EXPECT_TRUE(str_src1 == "   123 1234");
		}
		{
			string str_src1("   123 1234   ");

			str_src1.trim();
			EXPECT_TRUE(str_src1 == "123 1234");
		}
	}

	{
		{
			string str = u"   你 好 😙    "_o;
			EXPECT_TRUE(str.trim_start() == u"你 好 😙    "_o);
		}
		{
			string str = u"   你 好 😙    "_o;
			EXPECT_TRUE(str.trim_end() == u"   你 好 😙"_o);
		}
		{
			string str = u"   你 好 😙    "_o;
			EXPECT_TRUE(str.trim() == u"你 好 😙"_o);
		}
	}

	{
		string str = u"你 好 😙"_o;
		EXPECT_TRUE(str.trim_start() == u"你 好 😙"_o);
		EXPECT_TRUE(str.trim_end() == u"你 好 😙"_o);
		EXPECT_TRUE(str.trim() == u"你 好 😙"_o);
	}
	{
		{
			string str = u"       "_o;
			EXPECT_TRUE(str.trim_start() == u""_o);
			EXPECT_TRUE(str.is_empty());
		}
		{
			string str = u"       "_o;
			EXPECT_TRUE(str.trim_end() == u""_o);
			EXPECT_TRUE(str.is_empty());
		}
		{
			string str = u"       "_o;
			EXPECT_TRUE(str.trim() == u""_o);
			EXPECT_TRUE(str.is_empty());
		}
	}
}
