#include "pch.h"
#include "CppUnitTest.h"
#include "../ostring/string_view.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_string_view
{
	TEST_CLASS(test_construct)
	{
	public:

		TEST_METHOD(sv_construct_ansi)
		{
			using namespace ostr;

			string_view sv_inconst = "a";
			Assert::IsTrue(sv_inconst.length() == 1);

			constexpr string_view sv_constexpr = "bb";
			Assert::IsTrue(sv_constexpr.length() == 2);

			sv_inconst = "ccc";
			Assert::IsTrue(sv_inconst.length() == 3);

			sv_inconst = sv_constexpr;
			Assert::IsTrue(sv_inconst.length() == 2);
		}

		TEST_METHOD(sv_construct_wide)
		{
			using namespace ostr;

			string_view sv_inconst = L"a";
			Assert::IsTrue(sv_inconst.length() == 1);

			constexpr string_view sv_constexpr = L"bb";
			Assert::IsTrue(sv_constexpr.length() == 2);

			sv_inconst = L"ccc";
			Assert::IsTrue(sv_inconst.length() == 3);

			sv_inconst = sv_constexpr;
			Assert::IsTrue(sv_inconst.length() == 2);
		}
		TEST_METHOD(sv_compare)
		{
			using namespace ostr;

			string_view sv_wide_an = L"an";
			string_view sv_ansi_an = "an";
			Assert::IsTrue(sv_wide_an == sv_ansi_an);

			string_view sv_ansi_aa = "aa";
			Assert::IsFalse(sv_ansi_an == sv_ansi_aa);
			Assert::IsFalse(sv_wide_an == sv_ansi_aa);


			Assert::IsTrue(sv_ansi_aa < sv_ansi_an);
		}
		TEST_METHOD(sv_length)
		{
			using namespace ostr;

			string_view sv_wide_an = L"an";
			string_view sv_ansi_an = "an";
			Assert::AreEqual<size_t>(2, sv_ansi_an.length());
			Assert::AreEqual<size_t>(2, sv_wide_an.length());

			string_view sv_ansi_null = "";
			Assert::AreEqual<size_t>(0, sv_ansi_null.length());
		}
		TEST_METHOD(sv_remove_prefix_suffix)
		{
			using namespace ostr;

			{
				string_view sv_wide_an = L"anti-aliasing";
				sv_wide_an.remove_prefix(5);
				Assert::IsTrue(sv_wide_an == "aliasing");

				sv_wide_an.remove_suffix(3);
				Assert::IsTrue(sv_wide_an == "alias"); 
			}
			{
				string_view sv_wide_an = L"anti-aliasing";
				Assert::IsTrue(sv_wide_an.remove_prefix_copy(5) == "aliasing");

				Assert::IsTrue(sv_wide_an.remove_suffix_copy(3) == "anti-alias");
			}
		}
		TEST_METHOD(sv_substring)
		{
			using namespace ostr;

			{
				string_view sv_wide_an = L"anti-aliasing";
				Assert::IsTrue(sv_wide_an.substring(2) == "ti-aliasing");
			}
			{
				string_view sv_wide_an = L"anti-aliasing";
				Assert::IsTrue(sv_wide_an.substring(2, 5) == "ti-al");
			}
			{
				string_view sv_wide_an = L"anti-aliasing";
				Assert::IsTrue(sv_wide_an.substring(2, 0) == "");
			}
		}
		TEST_METHOD(sv_to_string)
		{
			using namespace ostr;
			
			string_view sv = "123"o.substring(0, 1);
			string s = sv.to_string();
			Assert::IsTrue(s == "1");
		}
	};
}