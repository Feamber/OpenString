#include "pch.h"
#include "CppUnitTest.h"
#include "../ostring/string.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
			string str_empty_wide			(L"");
			string str_ctor_ansi			("123321");
			string str_assign				= "123321";
			string str_ctor_wide			(L"123321");
			string str_ctor_ansi_another	("abccd");
			string str_ctor_hans			(L"我");
			string str_ctor_hans_another	(L"我他");
			string str_ctor_supplement		(L"𪚥😁");
			string str_ctor_supplement_2	(L"𪚥😁");
			string str_ctor_supplement_3	(L"𪚥");
			string str_ctor_6c_ansi			('c', 6);
			string str_ctor_6c_wide			(L'c', 6);
			string str_ctor_6c_strcopy		("c", 6);
			string str_ctor_6c_string		("cccccc");

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
			Assert::IsTrue	(str_ctor_6c_strcopy	== str_ctor_6c_string);
		}

		TEST_METHOD(string_compare)
		{
			using namespace ostr;

			string str_default_ctor;
			string str_empty_ansi("");
			string str_empty_wide(L"");
			string str_ctor_ansi("123321");
			string str_assign = "123321";
			string str_ctor_wide(L"123321");
			string str_ctor_ansi_another("abccd");
			string str_ctor_hans(L"我");
			string str_ctor_hans_another(L"我他");
			string str_ctor_supplement(L"𪚥😁");
			string str_ctor_supplement_2(L"𪚥😁");
			string str_ctor_supplement_3(L"𪚥");

			Assert::AreEqual(str_default_ctor.compare(str_empty_ansi), 0);
			Assert::AreEqual(str_empty_wide.compare(str_assign), -1);
			Assert::AreEqual(str_assign.compare(str_ctor_wide), 0);
			Assert::AreEqual(str_ctor_wide.compare(str_assign), 0);
			Assert::AreEqual(str_ctor_wide.compare(str_ctor_ansi_another), 1);
			Assert::AreEqual(str_ctor_hans.compare(str_ctor_hans_another), -1);
			Assert::AreEqual(str_ctor_supplement.compare(str_ctor_supplement_2), 0);
			Assert::AreEqual(str_ctor_supplement.compare(str_ctor_supplement_3), 1);
		}
	};

	TEST_CLASS(test_string_state)
	{
	public:

		TEST_METHOD(string_compare)
		{
			using namespace ostr;

			string str_default_ctor;
			string str_empty_ansi("");
			string str_empty_wide(L"");
			string str_ctor_ansi("123321");
			string str_assign = "123321";
			string str_ctor_wide(L"123321");
			string str_ctor_ansi_another("abccd");
			string str_ctor_hans(L"我");
			string str_ctor_hans_another(L"我他");
			string str_ctor_supplement(L"𪚥😁");
			string str_ctor_supplement_2(L"𪚥");

			Assert::AreEqual(str_default_ctor.length(), 0u);
			Assert::IsTrue(str_default_ctor.is_empty());
			Assert::AreEqual(str_empty_ansi.length(), 0u);
			Assert::AreEqual(str_empty_wide.length(), 0u);
			Assert::AreEqual(str_ctor_ansi.length(), 6u);
			Assert::IsFalse(str_ctor_ansi.is_empty());
			Assert::AreEqual(str_assign.length(), 6u);
			Assert::AreEqual(str_ctor_wide.length(), 6u);
			Assert::AreEqual(str_ctor_ansi_another.length(), 5u);
			Assert::AreEqual(str_ctor_hans.length(), 1u);
			Assert::AreEqual(str_ctor_hans_another.length(), 2u);
			Assert::AreEqual(str_ctor_supplement.length(), 2u);
			Assert::AreEqual(str_ctor_supplement_2.length(), 1u);
		}
	};

	TEST_CLASS(test_string_operator)
	{
	public:

		TEST_METHOD(string_combination)
		{
			using namespace ostr;

			const char char_wo[] = { 0xe6i8, 0x88i8, 0x91i8, 0x00i8 };
			string str_wo(char_wo, encoding::utf8);
			string str_ai(L"😘");
			string str_ni(L"ni");

			string combine = str_wo + str_ai + str_ai + str_ni;
			string correct = L"我😘😘ni";

			Assert::IsTrue(combine == correct);
		}
	};

}
