#include "pch.h"
#include "CppUnitTest.h"
#include "../ostring/helpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test_helper
{
	TEST_CLASS(test_unicode_helper)
	{
	public:

		TEST_METHOD(get_utf8_sequence_length)
		{
			using namespace ostr::helper::codepoint;
			{
				ostr::uint8_t mask;
				constexpr size_t expected_length = 1;
				Assert::AreEqual<>(utf8_sequence_length(0b00011111ui8, mask), expected_length);
				Assert::AreEqual(mask, 0b00000000ui8);
			}

			{
				ostr::uint8_t mask;
				constexpr size_t expected_length = 0;
				Assert::AreEqual(utf8_sequence_length(0b10000111ui8, mask), expected_length);
				Assert::AreEqual(mask, 0b10000000ui8);
			}

			{
				ostr::uint8_t mask;
				constexpr size_t expected_length = 2;
				Assert::AreEqual(utf8_sequence_length(0b11011100ui8, mask), expected_length);
				Assert::AreEqual(mask, 0b11000000ui8);
			}

			{
				ostr::uint8_t mask;
				constexpr size_t expected_length = 3;
				Assert::AreEqual(utf8_sequence_length(0b11100100ui8, mask), expected_length);
				Assert::AreEqual(mask, 0b11100000ui8);
			}
		}

		TEST_METHOD(convert_utf16_to_utf32)
		{
			using namespace ostr::helper::codepoint;
			{
				size_t length;
				uint32_t value;
				constexpr size_t expected_length = 1;
				const ostr::wide_char wide_chars[] = L"2";
				utf16_to_utf32((const uint16_t*)wide_chars, length, value);
				Assert::AreEqual(value, 50u);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				uint32_t value;
				constexpr size_t expected_length = 1;
				const ostr::wide_char wide_chars[] = L"b";
				utf16_to_utf32((const uint16_t*)wide_chars, length, value);
				Assert::AreEqual(value, 98u);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				uint32_t value;
				constexpr size_t expected_length = 1;
				const ostr::wide_char wide_chars[] = L"吐";
				utf16_to_utf32((const uint16_t*)wide_chars, length, value);
				Assert::AreEqual(value, 21520u);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				uint32_t value;
				constexpr size_t expected_length = 2;
				const ostr::wide_char wide_chars[] = L"😘";
				utf16_to_utf32((const uint16_t*)wide_chars, length, value);
				Assert::AreEqual(value, 0x1f618u);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				uint32_t value;
				constexpr size_t expected_length = 2;
				const ostr::wide_char wide_chars[] = L"𪚥";
				utf16_to_utf32((const uint16_t*)wide_chars, length, value);
				Assert::AreEqual(value, 0x2a6a5u);
				Assert::AreEqual(length, expected_length);
			}
		}

		TEST_METHOD(convert_utf8_to_utf32)
		{
			using namespace ostr::helper::codepoint;
			{
				size_t length;
				uint32_t utf32;
				constexpr size_t expected_length = 1;
				const ostr::ansi_char ansi_chars[] = "1";
				utf8_to_utf32((const uint8_t*)ansi_chars, length, utf32);
				Assert::AreEqual(utf32, 49u);
				Assert::AreEqual(length, expected_length);
			}

			{
				size_t length;
				uint32_t utf32;
				constexpr size_t expected_length = 1;
				const ostr::ansi_char ansi_chars[] = "a";
				utf8_to_utf32((const uint8_t*)ansi_chars, length, utf32);
				Assert::AreEqual(utf32, 97u);
				Assert::AreEqual(length, expected_length);
			}

			{
				size_t length;
				uint32_t utf32;
				constexpr size_t expected_length = 3;
				const ostr::ansi_char ansi_chars[] = { 0xe6i8, 0x88i8, 0x91i8, 0x00i8 };
				ostr::wide_char* wide_chars = L"我";
				utf8_to_utf32((const uint8_t*)ansi_chars, length, utf32);
				Assert::AreEqual(utf32, (uint32_t)wide_chars[0]);
				Assert::AreEqual(length, expected_length);
			}

			{
				size_t length;
				uint32_t utf32;
				constexpr size_t expected_length = 4;
				// utf8 for 😁
				const ostr::ansi_char ansi_chars[] = { 0xf0i8, 0x9fi8, 0x98i8, 0x81i8, 0x00i8 };
				utf8_to_utf32((const uint8_t*)ansi_chars, length, utf32);
				Assert::AreEqual(utf32, 0x1f601u);
				Assert::AreEqual(length, expected_length);
			}

			{
				size_t length;
				uint32_t utf32;
				constexpr size_t expected_length = 4;
				// utf8 for 𪚥
				const ostr::ansi_char ansi_chars[] = { 0xf0i8, 0xaai8, 0x9ai8, 0xa5i8, 0x00i8 };
				utf8_to_utf32((const uint8_t*)ansi_chars, length, utf32);
				Assert::AreEqual(utf32, 0x2a6a5u);
				Assert::AreEqual(length, expected_length);
			}
		}
	};

	TEST_CLASS(test_string_helper)
	{
	public:

		TEST_METHOD(string_compare)
		{
			using namespace ostr::helper::string;

			const char* str1 = "123321";
			const char* str2 = "123321";
			const char* str3 = "123";
			const wchar_t* str4 = L"123321";
			const wchar_t* str5 = L"123321";
			const wchar_t* str6 = L"我";
			const wchar_t* str7 = L"我";
			const wchar_t* str8 = L"我他";
			const wchar_t* str9 = L"𪚥😁";
			const wchar_t* str10 = L"𪚥😁";
			const wchar_t* str11 = L"𪚥";

			Assert::AreEqual(compare(str1, str1), 0);
			Assert::AreEqual(compare(str1, str2), 0);
			Assert::AreEqual(compare(str2, str1), 0);
			Assert::AreEqual(compare(str1, str3), 1);
			Assert::AreEqual(compare(str3, str1), -1);
			Assert::AreEqual(compare(str4, str4), 0);
			Assert::AreEqual(compare(str4, str5), 0);
			Assert::AreEqual(compare(str5, str4), 0);
			Assert::AreEqual(compare(str5, str6), -1);
			Assert::AreEqual(compare(str6, str5), 1);
			Assert::AreEqual(compare(str6, str7), 0);
			Assert::AreEqual(compare(str7, str8), -1);
			Assert::AreEqual(compare(str8, str7), 1);
			Assert::AreEqual(compare(str9, str10), 0);
			Assert::AreEqual(compare(str10, str9), 0);
			Assert::AreEqual(compare(str9, str11), 1);
			Assert::AreEqual(compare(str11, str9), -1);
		}
	};
}
