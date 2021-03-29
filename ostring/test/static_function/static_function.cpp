/*#include "pch.h"
#include "CppUnitTest.h"
#include <chrono>
#include "../fmt/include/fmt/chrono.h"
#include <vector>
#include "../fmt/include/fmt/ranges.h"
#include "../ostring/static_functions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct complex
{
	double a;
	double b;
};

template<>
struct fmt::formatter<complex, char16_t>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(complex number, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), u"{0}+i{1}", number.a, number.b);
	}
};

namespace test_static_function
{
	TEST_CLASS(test_format)
	{
	public:

		TEST_METHOD(sv_format)
		{
			using namespace ostr;
			using namespace std::chrono_literals;

			// int
			string a42 = format(OSTR("The number is {}."), 42);
			Assert::IsTrue(a42 == "The number is 42.");

			// const char16_t*
			string a44 = format(OSTR("The number is {}."), OSTR("43"));
			Assert::IsTrue(a44 == "The number is 43.");

			// custom structure
			string a45 = OSTR("The complex is {}.").format(complex{1, 2});
			Assert::IsTrue(a45 == "The complex is 1+i2.");

			// syntax for string_view
			string a46 = OSTR("The number is {}.").format(OSTR("46"));
			Assert::IsTrue(a46 == "The number is 46.");

			// float with correct rounding
			string a47 = format(OSTR("The number is {}."), 47.0);
			Assert::IsTrue(a47 == "The number is 47.");

			// format ordered
			string a48 = format(OSTR("The number is {1} and {0}."), 48.0f, 49);
			Assert::IsTrue(a48 == "The number is 49 and 48.");

			// time
			string time = OSTR("strftime-like format: {:%H:%M:%S}").format(3h + 15min + 30s);
			Assert::IsTrue(time == "strftime-like format: 03:15:30");
		}
	};
}
*/