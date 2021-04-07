
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

TEST(format, one_param)
{
	// TODO

	using namespace ostr;
	using namespace std::literals;

	// literal
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

TEST(format, alignment)
{
	using namespace ostr;
	using namespace std::literals;

	EXPECT_TRUE(ofmt::format(u"{0}{1}{2}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,5}{1,5}{2,5}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"   alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,-5}{1,-5}{2,-5}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"al   alignalignment"sv);

	EXPECT_TRUE(ofmt::format(u"{0}{1}{2}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,5}{1,5}{2,5}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"   alalignalignment"sv);
	EXPECT_TRUE(ofmt::format(u"{0,-5}{1,-5}{2,-5}"sv, u"al"sv, u"align"sv, u"alignment"sv) == u"al   alignalignment"sv);
}
