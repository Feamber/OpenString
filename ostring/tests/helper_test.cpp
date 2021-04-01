
#include "ostring/helpers.h"

/*namespace test_helper
{
	TEST_CLASS(test_unicode_helper)
	{
	public:

		TEST_METHOD(get_utf8_sequence_length)
		{
			using namespace ostr::helper::codepoint;
			{
				ostr::char8_t mask;
				constexpr size_t expected_length = 1;
				Assert::AreEqual(utf8_sequence_length(0b00011111ui8, mask), expected_length);
				Assert::AreEqual(mask, 0b00000000ui8);
			}
			
			{
				ostr::char8_t mask;
				constexpr size_t expected_length = 0;
				Assert::AreEqual(utf8_sequence_length(0b10000111ui8, mask), expected_length);
				Assert::AreEqual(mask, 0b10000000ui8);
			}

			{
				ostr::char8_t mask;
				constexpr size_t expected_length = 2;
				Assert::AreEqual(utf8_sequence_length(0b11011100ui8, mask), expected_length);
				Assert::AreEqual(mask, 0b11000000ui8);
			}

			{
				ostr::char8_t mask;
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
				char32_t value;
				constexpr size_t expected_length = 1;
				const wchar_t wide_chars[] = L"2";
				utf16_to_utf32((const char16_t*)wide_chars, length, value);
				Assert::AreEqual<uint32_t>(50, value);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				char32_t value;
				constexpr size_t expected_length = 1;
				const wchar_t wide_chars[] = L"b";
				utf16_to_utf32((const char16_t*)wide_chars, length, value);
				Assert::AreEqual<uint32_t>(98, value);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				char32_t value;
				constexpr size_t expected_length = 1;
				const wchar_t wide_chars[] = L"吐";
				utf16_to_utf32((const char16_t*)wide_chars, length, value);
				Assert::AreEqual<uint32_t>(21520, value);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				char32_t value;
				constexpr size_t expected_length = 2;
				const wchar_t wide_chars[] = L"😘";
				utf16_to_utf32((const char16_t*)wide_chars, length, value);
				Assert::AreEqual<uint32_t>(0x1f618, value);
				Assert::AreEqual(length, expected_length);
			}
			{
				size_t length;
				char32_t value;
				constexpr size_t expected_length = 2;
				const wchar_t wide_chars[] = L"𪚥";
				utf16_to_utf32((const char16_t*)wide_chars, length, value);
				Assert::AreEqual<uint32_t>(0x2a6a5, value);
				Assert::AreEqual(length, expected_length);
			}
		}

		TEST_METHOD(convert_utf8_to_utf32)
		{
			using namespace ostr::helper::codepoint;
			{
				size_t length;
				char32_t utf32;
				constexpr size_t expected_length = 1;
				const char ansi_chars[] = "1";
				utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
				Assert::AreEqual<uint32_t>(49, utf32);
				Assert::AreEqual(expected_length, length);
			}

			{
				size_t length;
				char32_t utf32;
				constexpr size_t expected_length = 1;
				const char ansi_chars[] = "a";
				utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
				Assert::AreEqual<uint32_t>(97, utf32);
				Assert::AreEqual(expected_length, length);
			}

			{
				size_t length;
				char32_t utf32;
				constexpr size_t expected_length = 3;
				const char ansi_chars[] = { 0xe6i8, 0x88i8, 0x91i8, 0x00i8 };
				wchar_t* wide_chars = L"我";
				utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
				Assert::AreEqual<uint32_t>((char32_t)wide_chars[0], utf32);
				Assert::AreEqual(expected_length, length);
			}

			{
				size_t length;
				char32_t utf32;
				constexpr size_t expected_length = 4;
				// utf8 for 😁
				const char ansi_chars[] = { 0xf0i8, 0x9fi8, 0x98i8, 0x81i8, 0x00i8 };
				utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
				Assert::AreEqual<uint32_t>(0x1f601u, utf32);
				Assert::AreEqual(expected_length, length);
			}

			{
				size_t length;
				char32_t utf32;
				constexpr size_t expected_length = 4;
				// utf8 for 𪚥
				const char ansi_chars[] = { 0xf0i8, 0xaai8, 0x9ai8, 0xa5i8, 0x00i8 };
				utf8_to_utf32((const ostr::char8_t*)ansi_chars, length, utf32);
				Assert::AreEqual<uint32_t>(0x2a6a5u, utf32);
				Assert::AreEqual(expected_length, length);
			}
		}
	};

	TEST_CLASS(test_string_helper)
	{
	public:

	};
}*/
