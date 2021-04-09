
#include <gtest/gtest.h>
#include <string_view>

#include "ostring/types.h"
#include "ostring/helpers.h"


TEST(helper, lowercase)
{
	using namespace ostr;

	EXPECT_TRUE(ostr::helper::character::char_lowercase('A') == 'a');
	EXPECT_TRUE(ostr::helper::character::char_lowercase('a') == 'a');
	EXPECT_TRUE(ostr::helper::character::char_lowercase('1') == '1');
	EXPECT_TRUE(ostr::helper::character::char_lowercase(u'A') == u'a');
	EXPECT_TRUE(ostr::helper::character::char_lowercase(u'a') == u'a');
	EXPECT_TRUE(ostr::helper::character::char_lowercase(u'1') == u'1');
	EXPECT_TRUE(ostr::helper::character::char_lowercase(u'我') == u'我');
	EXPECT_TRUE(ostr::helper::character::char_lowercase(u'♂') == u'♂');
	EXPECT_TRUE(ostr::helper::character::char_lowercase(U'😙') == U'😙');

}

TEST(helper, get_utf8_sequence_length)
{
	using namespace ostr;
	using namespace ostr::helper::codepoint;
	{
		ostr::char8_t mask;
		constexpr small_size_t expected_length = 1;
		EXPECT_EQ(utf8_sequence_length(0b00011111ui8, mask), expected_length);
		EXPECT_EQ(mask, 0b00000000ui8);
	}

	{
		ostr::char8_t mask;
		constexpr small_size_t expected_length = 0;
		EXPECT_EQ(utf8_sequence_length(0b10000111ui8, mask), expected_length);
		EXPECT_EQ(mask, 0b10000000ui8);
	}

	{
		ostr::char8_t mask;
		constexpr small_size_t expected_length = 2;
		EXPECT_EQ(utf8_sequence_length(0b11011100ui8, mask), expected_length);
		EXPECT_EQ(mask, 0b11000000ui8);
	}

	{
		ostr::char8_t mask;
		constexpr small_size_t expected_length = 3;
		EXPECT_EQ(utf8_sequence_length(0b11100100ui8, mask), expected_length);
		EXPECT_EQ(mask, 0b11100000ui8);
	}
}

TEST(helper, convert_utf16_to_utf32)
{
	using namespace ostr;
	using namespace ostr::helper::codepoint;
	{
		small_size_t length;
		char32_t value;
		constexpr small_size_t expected_length = 1;
		const char16_t src[] = u"2";
		utf16_to_utf32((const char16_t*)src, length, value);
		EXPECT_EQ(U'2', value);
		EXPECT_EQ(length, expected_length);
	}
	{
		small_size_t length;
		char32_t value;
		constexpr small_size_t expected_length = 1;
		const char16_t src[] = u"吐";
		utf16_to_utf32((const char16_t*)src, length, value);
		EXPECT_EQ(U'吐', value);
		EXPECT_EQ(length, expected_length);
	}
	{
		small_size_t length;
		char32_t value;
		constexpr small_size_t expected_length = 2;
		const char16_t src[] = u"😘";
		utf16_to_utf32((const char16_t*)src, length, value);
		EXPECT_EQ(U'😘', value);
		EXPECT_EQ(length, expected_length);
	}
	{
		small_size_t length;
		char32_t value;
		constexpr small_size_t expected_length = 2;
		const char16_t src[] = u"𪚥";
		utf16_to_utf32((const char16_t*)src, length, value);
		EXPECT_EQ(U'𪚥', value);
		EXPECT_EQ(length, expected_length);
	}
}

TEST(helper, convert_utf32_to_utf16)
{
	using namespace ostr;
	using namespace ostr::helper::codepoint;
	{
		small_size_t length;
		constexpr small_size_t expected_length = 1;
		const char32_t src = U'2';
		surrogate_pair sp;
		utf32_to_utf16(src, sp, length);
		EXPECT_EQ(u'2', sp[0]);
		EXPECT_EQ(0, sp[1]);
		EXPECT_EQ(length, expected_length);
	}
	{
		small_size_t length;
		constexpr small_size_t expected_length = 1;
		const char32_t src = U'我';
		surrogate_pair sp;
		utf32_to_utf16(src, sp, length);
		EXPECT_EQ(u'我', sp[0]);
		EXPECT_EQ(0, sp[1]);
		EXPECT_EQ(length, expected_length);
	}
	{
		small_size_t length;
		constexpr small_size_t expected_length = 2;
		const char32_t src = U'😁';
		surrogate_pair sp;
		utf32_to_utf16(src, sp, length);
		EXPECT_EQ(u'\xD87D', sp[0]);
		EXPECT_EQ(u'\xDE01', sp[1]);
		EXPECT_EQ(length, expected_length);
	}
}

TEST(helper, convert_utf8_to_utf32)
{
	using namespace ostr;
	using namespace ostr::helper::codepoint;
	{
		small_size_t length;
		char32_t utf32;
		constexpr small_size_t expected_length = 1;
		const char ansi_chars[] = "1";
		utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
		EXPECT_EQ(U'1', utf32);
		EXPECT_EQ(expected_length, length);
	}

	{
		small_size_t length;
		char32_t utf32;
		constexpr small_size_t expected_length = 3;
		const char ansi_chars[] = { 0xe6i8, 0x88i8, 0x91i8, 0x00i8 };
		utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
		EXPECT_EQ(U'我', utf32);
		EXPECT_EQ(expected_length, length);
	}

	{
		small_size_t length;
		char32_t utf32;
		constexpr small_size_t expected_length = 4;
		const char ansi_chars[] = u8"𪚥";
		utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
		EXPECT_EQ(U'𪚥', utf32);
		EXPECT_EQ(expected_length, length);
	}
}

TEST(helper, crc32)
{
	using namespace ostr::helper::hash;
	using namespace std::literals;

	constexpr uint32_t hc = hash_crc32("stack-overflow"sv);
	EXPECT_EQ(hc, 0x335CC04A);
	EXPECT_EQ(hash_crc32(u"stack-overflow"sv), 0x335CC04A);
	const char* str = "stack-overflow";
	EXPECT_EQ(hash_crc32(std::string_view(str)), 0x335CC04A);
}

TEST(helper, from_float)
{
	using namespace ostr::helper::string;

	std::string fstr;
	from_float_round(3.141, fstr);
	EXPECT_TRUE(fstr == "3.141");
}