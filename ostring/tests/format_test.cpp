
#include <gtest/gtest.h>

#include "ostring/types.h"
#include "ostring/format.h"

#include <chrono>
#include <iostream>
#include <array>
#include "fmt/format.h"

TEST(format, exec)
{
	using namespace ostr;
	using namespace ofmt;
	using namespace std::literals;

	{
		auto t0 = std::chrono::system_clock::now();
		for (int i = 0; i < 100000; ++i) {
			std::u16string str = ostr::ofmt::format(u"{{}}{0}}}{1}2"sv, -111, 115);
		}
		auto t1 = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = t1 - t0;
		std::cout << delta.count() << std::endl;
	}
	{
		auto t0 = std::chrono::system_clock::now();
		for (int i = 0; i < 100000; ++i) {
			std::u16string str = fmt::format(u"{{}}{0}}}{1}2"sv, -111, 115);
		}
		auto t1 = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = t1 - t0;
		std::cout << delta.count() << std::endl;
	}
}

TEST(format, no_param)
{
	using namespace ostr;
	using namespace std::literals;

	EXPECT_TRUE(ofmt::format(u"alignment") == u"alignment"sv);
	EXPECT_TRUE(ofmt::format(u"对齐") == u"对齐"sv);
	EXPECT_TRUE(ofmt::format(u"𪚥♂😁") == u"𪚥♂😁"sv);
}

TEST(format, one_param_manual)
{
	// TODO

	using namespace ostr;
	using namespace std::literals;

	// literal
	EXPECT_TRUE(ofmt::format(u"对{0}齐", "1"sv) == u"对1齐"sv);
	EXPECT_TRUE(ofmt::format(u"对{0}齐", "1"sv) == u"对1齐"sv);
	EXPECT_TRUE(ofmt::format(u"align{0}ment", u"你好𪚥"sv) == u"align你好𪚥ment"sv);
	EXPECT_TRUE(ofmt::format(u"𪚥♂{0}😁", u"你1") == u"𪚥♂你1😁"sv); 

	// c_str
	{
		const char* c_str = "123";
		EXPECT_TRUE(ofmt::format(u"对{0}齐", c_str) == u"对123齐"sv);
	}
	{
		const wchar_t* c_str = L"你1";
		EXPECT_TRUE(ofmt::format(u"𪚥♂{0}😁", c_str) == u"𪚥♂你1😁"sv);
	}
	{
		const char16_t* c_str = u"你好𪚥";
		EXPECT_TRUE(ofmt::format(u"align{0}ment", c_str) == u"align你好𪚥ment"sv);
	}
}

TEST(format, multi_param_manual)
{
	// TODO

	using namespace ostr;
	using namespace std::literals;

	// literal
	EXPECT_TRUE(ofmt::format(u"对{0}{1}齐", "1"sv, 3) == u"对13齐"sv);
	EXPECT_TRUE(ofmt::format(u"对{1}齐{0}", "1"sv, -2) == u"对-2齐1"sv);
	EXPECT_TRUE(ofmt::format(u"align{0}ment{2}{2}", u"你好𪚥"sv, 3, u"😁"sv) == u"align你好𪚥ment😁😁"sv);
	EXPECT_TRUE(ofmt::format(u"𪚥♂{0}{0}😁", u"你1") == u"𪚥♂你1你1😁"sv);

	// c_str
	{
		const char* c_str1 = "123";
		const wchar_t* c_str2 = L"你1";
		EXPECT_TRUE(ofmt::format(u"对{0}齐{1}", c_str1, c_str2) == u"对123齐你1"sv);
	}
	{
		const char16_t* c_str1 = u"你好𪚥";
		const wchar_t* c_str2 = L"你1";
		EXPECT_TRUE(ofmt::format(u"𪚥{1}♂{0}😁", c_str1, c_str2) == u"𪚥你1♂你好𪚥😁"sv);
	}
	{
		const char16_t* c_str = u"你好𪚥";
		EXPECT_TRUE(ofmt::format(u"align{0}ment{0}", c_str) == u"align你好𪚥ment你好𪚥"sv);
	}
}

TEST(format, one_param_auto)
{
	// TODO

	using namespace ostr;
	using namespace std::literals;

	// literal
	EXPECT_TRUE(ofmt::format(u"对{}齐", "1"sv) == u"对1齐"sv);
	EXPECT_TRUE(ofmt::format(u"对{}齐", "1"sv) == u"对1齐"sv);
	EXPECT_TRUE(ofmt::format(u"align{}ment", u"你好𪚥"sv) == u"align你好𪚥ment"sv);
	EXPECT_TRUE(ofmt::format(u"𪚥♂{}😁", u"你1") == u"𪚥♂你1😁"sv);

	// c_str
	{
		const char* c_str = "123";
		EXPECT_TRUE(ofmt::format(u"对{}齐", c_str) == u"对123齐"sv);
	}
	{
		const wchar_t* c_str = L"你1";
		EXPECT_TRUE(ofmt::format(u"𪚥♂{}😁", c_str) == u"𪚥♂你1😁"sv);
	}
	{
		const char16_t* c_str = u"你好𪚥";
		EXPECT_TRUE(ofmt::format(u"align{}ment", c_str) == u"align你好𪚥ment"sv);
	}
}

TEST(format, multi_param_auto)
{
	// TODO

	using namespace ostr;
	using namespace std::literals;

	// literal
	EXPECT_TRUE(ofmt::format(u"对{}齐{}", "1"sv, 3) == u"对1齐3"sv);
	EXPECT_TRUE(ofmt::format(u"对{}{}齐", "1"sv, -2) == u"对1-2齐"sv);
	EXPECT_TRUE(ofmt::format(u"ali{}gn{}ment", u""sv, u"你好𪚥"sv) == u"align你好𪚥ment"sv);
	EXPECT_TRUE(ofmt::format(u"{}𪚥♂{}😁", u"你1", u""sv) == u"你1𪚥♂😁"sv);

	// c_str
	{
		const char* c_str1 = "123";
		const char16_t* c_str2 = u"你好𪚥";
		EXPECT_TRUE(ofmt::format(u"对{}齐{}", c_str1, c_str2) == u"对123齐你好𪚥"sv);
	}
	{
		const wchar_t* c_str1 = L"你1";
		const char* c_str2 = "123";
		EXPECT_TRUE(ofmt::format(u"𪚥♂{}{}😁", c_str2, c_str1) == u"𪚥♂123你1😁"sv);
	}
	{
		const char16_t* c_str1 = u"你好𪚥";
		const char16_t* c_str2 = u"♂";
		EXPECT_TRUE(ofmt::format(u"align{}{}ment", c_str1, c_str2) == u"align你好𪚥♂ment"sv);
	}
}

TEST(format, alignment)
{
	using namespace ostr;
	using namespace std::literals;

	EXPECT_TRUE(ofmt::format(u"{0}{1}{2}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,5}{1,5}{2,5}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"   alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,-5}{1,-5}{2,-5}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"al   alignalignment"sv);

	EXPECT_TRUE(ofmt::format(u"{0}{1}{2}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,5}{1,5}{2,5}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"   alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,-5}{1,-5}{2,-5}"sv, u"al"sv, u"align"sv, "alignment"sv) == u"al   alignalignment"sv);
}
