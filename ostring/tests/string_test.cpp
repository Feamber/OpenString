﻿
#include <gtest/gtest.h>

#include "ostring/ostr.h"

namespace ostr {
	TEST(construct, literal) {
		using namespace ostr;
		using namespace ostr::literal;

		string str_inconst = u"a"_o;
		EXPECT_EQ(1, str_inconst.length());

		string sv_constexpr = u"bb"_o;
		EXPECT_EQ(2, sv_constexpr.length());

		str_inconst = u"你😀"_o;
		EXPECT_EQ(2, str_inconst.length());

		str_inconst = sv_constexpr;
		EXPECT_EQ(2, str_inconst.length());
	}
}


/*using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_string
{
	TEST_CLASS(test_string_base)
	{
	public:

		TEST_METHOD(string_construct)
		{
			using namespace ostr;

			string str_default_ctor;
			string str_empty_ansi			("");
			string str_empty_wide			(OSTR(""));
			string str_ctor_ansi			("123321");
			string str_assign				= "123321";
			string str_ctor_wide			(OSTR("123321"));
			string str_ctor_ansi_another	("abccd");
			string str_ctor_hans			(OSTR("我"));
			string str_ctor_hans_another	(OSTR("我他"));
			string str_ctor_supplement		(OSTR("𪚥😁"));
			string str_ctor_supplement_2	(OSTR("𪚥😁"));
			string str_ctor_supplement_3	(OSTR("𪚥"));
			string str_ctor_6c_ansi			('c', 6);
			string str_ctor_6c_wide			(L'c', 6);
			string str_ctor_6c_string		("cccccc");
			string str_ctor_substring1		("123321", 3);
			string str_ctor_substring2		(OSTR("123"));

			Assert::IsTrue	(str_default_ctor		== str_default_ctor);
			Assert::IsTrue	(str_default_ctor		== str_empty_ansi);
			Assert::IsTrue	(str_empty_ansi			== str_empty_wide);
			Assert::IsTrue	(str_default_ctor		== str_empty_wide);
			Assert::IsTrue	(str_ctor_ansi			== str_ctor_ansi);
			Assert::IsTrue	(str_ctor_ansi			== str_assign);
			Assert::IsTrue	(str_assign				== str_ctor_wide);
			Assert::IsTrue	(str_ctor_ansi			== str_ctor_wide);
			Assert::IsTrue	(str_ctor_wide			== str_ctor_wide);
			Assert::IsFalse	(str_ctor_ansi			== str_ctor_ansi_another);
			Assert::IsFalse	(str_ctor_hans			== str_ctor_hans_another);
			Assert::IsTrue	(str_ctor_supplement	== str_ctor_supplement_2);
			Assert::IsFalse	(str_ctor_supplement	== str_ctor_supplement_3);
			Assert::IsTrue	(str_ctor_6c_ansi		== str_ctor_6c_string);
			Assert::IsTrue	(str_ctor_6c_wide		== str_ctor_6c_string);
			Assert::IsTrue	(str_ctor_substring1	== str_ctor_substring2);
		}

		TEST_METHOD(string_compare)
		{
			using namespace ostr;

			string str_default_ctor;
			string str_empty_ansi("");
			string str_empty_wide(OSTR(""));
			string str_ctor_ansi("123321");
			string str_assign = "123321";
			string str_ctor_wide(OSTR("123321"));
			string str_ctor_ansi_another("abccd");
			string str_ctor_hans(OSTR("我"));
			string str_ctor_hans_another(OSTR("我他"));
			string str_ctor_supplement(OSTR("𪚥😁"));
			string str_ctor_supplement_2(OSTR("𪚥😁"));
			string str_ctor_supplement_3(OSTR("𪚥"));

			Assert::AreEqual(0, str_default_ctor.compare(str_empty_ansi));
			Assert::AreEqual(-1, str_empty_wide.compare(str_assign));
			Assert::AreEqual(0, str_assign.compare(str_ctor_wide));
			Assert::AreEqual(0, str_ctor_wide.compare(str_assign));
			Assert::AreEqual(-1, str_ctor_wide.compare(str_ctor_ansi_another));
			Assert::AreEqual(-1, str_ctor_hans.compare(str_ctor_hans_another));
			Assert::AreEqual(0, str_ctor_supplement.compare(str_ctor_supplement_2));
			Assert::AreEqual(1, str_ctor_supplement.compare(str_ctor_supplement_3));
		}
	};

	TEST_CLASS(test_string_state)
	{
	public:

		TEST_METHOD(string_length)
		{
			using namespace ostr;

			string str_default_ctor;
			string str_empty_ansi("");
			string str_empty_wide(OSTR(""));
			string str_ctor_ansi("123321");
			string str_assign = "123321";
			string str_ctor_wide(OSTR("123321"));
			string str_ctor_ansi_another("abccd");
			string str_ctor_hans(OSTR("我"));
			string str_ctor_hans_another(OSTR("我他"));
			string str_ctor_supplement(OSTR("𪚥😁"));
			string str_ctor_supplement_2(OSTR("𪚥"));

			Assert::AreEqual<size_t>	(str_default_ctor.length(),			0);
			Assert::IsTrue				(str_default_ctor.is_empty());
			Assert::AreEqual<size_t>	(str_empty_ansi.length(),			0);
			Assert::AreEqual<size_t>	(str_empty_wide.length(),			0);
			Assert::AreEqual<size_t>	(str_ctor_ansi.length(),			6);
			Assert::IsFalse				(str_ctor_ansi.is_empty());
			Assert::AreEqual<size_t>	(str_assign.length(),				6);
			Assert::AreEqual<size_t>	(str_ctor_wide.length(),			6);
			Assert::AreEqual<size_t>	(str_ctor_ansi_another.length(),	5);
			Assert::AreEqual<size_t>	(str_ctor_hans.length(),			1);
			Assert::AreEqual<size_t>	(str_ctor_hans_another.length(),	2);
			Assert::AreEqual<size_t>	(str_ctor_supplement.length(),		2);
			Assert::AreEqual<size_t>	(str_ctor_supplement_2.length(),	1);
		}
	};

	TEST_CLASS(test_string_operator)
	{
	public:

		TEST_METHOD(string_combination)
		{
			using namespace ostr;

			{
				const char char_wo[] = { 0xe6i8, 0x88i8, 0x91i8, 0x00i8 };
				string str_wo(char_wo, SIZE_MAX, encoding::utf8);
				string str_ai(OSTR("😘"));
				string str_ni(OSTR("ni"));

				string combine = str_wo + str_ai + str_ai + str_ni;
				string correct = OSTR("我😘😘ni");

				Assert::IsTrue(combine == correct); 
			}
		}
	};

	TEST_CLASS(test_string_find)
	{
	public:

		TEST_METHOD(string_substring)
		{
			using namespace ostr;

			{
				string str_src1("123321");

				Assert::IsTrue(str_src1.substring(3) == string("321"));
				Assert::IsTrue(str_src1.substring(0, 4) == string("1233"));
				Assert::IsTrue(str_src1.substring(1, 4) == string("2332"));

			}

			{
				string str_src2(OSTR("233你abc"));

				Assert::IsTrue(str_src2.substring(3) == string(OSTR("你abc")));
				Assert::IsTrue(str_src2.substring(0, 4) == string(OSTR("233你")));
				Assert::IsTrue(str_src2.substring(1, 4) == string(OSTR("33你a")));
			}

			{
				string str_src3(OSTR("我😘😘ni"));

				Assert::IsTrue(str_src3.substring(2) == string(OSTR("😘ni")));
				Assert::IsTrue(str_src3.substring(0, 4) == string(OSTR("我😘😘n")));
				Assert::IsTrue(str_src3.substring(1, 3) == string(OSTR("😘😘n")));
			}
		}

		TEST_METHOD(string_lowercase)
		{
			using namespace ostr;

			Assert::IsTrue(ostr::helper::character::char_lowercase('A') == 'a');
			Assert::IsTrue(ostr::helper::character::char_lowercase('a') == 'a');
			Assert::IsTrue(ostr::helper::character::char_lowercase('1') == '1');
			Assert::IsTrue(ostr::helper::character::char_lowercase(L'A') == L'a');
			Assert::IsTrue(ostr::helper::character::char_lowercase(L'a') == L'a');
			Assert::IsTrue(ostr::helper::character::char_lowercase(L'1') == L'1');
			Assert::IsTrue(ostr::helper::character::char_lowercase(L'我') == L'我');

		}
		TEST_METHOD(string_index_of)
		{
			using namespace ostr;

			{
				string str_src1("1231234");
				string str_to_find1("123");
				string str_to_find2("1234");

				Assert::AreEqual<size_t>(0, str_src1.index_of(str_to_find1));
				Assert::AreEqual<size_t>(3, str_src1.index_of(str_to_find2));
				Assert::AreEqual<size_t>(3, str_src1.index_of(str_to_find1, 2));
				Assert::AreEqual<size_t>(3, str_src1.index_of(str_to_find1, 2, 5)); 
			}

			{
				string str_src1(OSTR("我😘😘ni"));
				string str_to_find1(OSTR("😘"));
				string str_to_find2(OSTR("😘n"));
				string str_to_find3("n");

				Assert::AreEqual<size_t>(1, str_src1.index_of(str_to_find1));
				Assert::AreEqual<size_t>(3, str_src1.index_of(str_to_find3));
				Assert::AreEqual<size_t>(2, str_src1.index_of(str_to_find1, 2));
				Assert::AreEqual<size_t>(2, str_src1.index_of(str_to_find2, 2, 2));
			}
		}
		TEST_METHOD(string_search)
		{
			using namespace ostr;

			{
				string str("123456789");

				Assert::AreEqual<size_t>(str.search([](wchar_t c) {return c > L'4'; }), 4);
			}
		}
		TEST_METHOD(string_replace)
		{
			using namespace ostr;

			{
				string str_src1("1231234");

				str_src1.replace(3, 2, "");
				Assert::IsTrue(str_src1 == "12334");
			}
			{
				string str_src1("1231234");

				Assert::IsTrue(str_src1.replace_copy("123", OSTR("1234")) == "123412344");
			}
		}
		TEST_METHOD(string_format)
		{
			using namespace ostr;

			{
				string str_src1("123{}1234");

				string str = str_src1.format(OSTR("a"));
				Assert::IsTrue(str == "123a1234");
			}
			{
				string str_src1("123{1}12{0}34");

				string str = str_src1.format(123, OSTR("a"));
				Assert::IsTrue(str == "123a1212334");
			}
			{
				string str_src1("123{2}12{1}34");

				string str = str_src1.format(OSTR("a"), 123, OSTR("b"));
				Assert::IsTrue(str == "123b1212334");
			}
		}
		TEST_METHOD(string_trim)
		{
			using namespace ostr;

			{
				string str_src1("   1231234");

				str_src1.trim_start();
				Assert::IsTrue(str_src1 == "1231234");
			}
			{
				string str_src1("   1231234   ");

				str_src1.trim_end();
				Assert::IsTrue(str_src1 == "   1231234");
			}
			{
				string str_src1("   1231234   ");

				str_src1.trim();
				Assert::IsTrue(str_src1 == "1231234");
			}
		}
	};


}
*/