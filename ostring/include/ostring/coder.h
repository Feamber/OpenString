#pragma once
#include "definitions.h"
#include "helpers.h"
#include <string>
#include <string_view>

_NS_OSTR_BEGIN

namespace coder {

	OPEN_STRING_EXPORT
	inline bool convert_append(std::string_view sv8, std::u16string& out_u16)
	{
		out_u16.reserve(out_u16.size() + sv8.size());
		size_t utf8_length;
		size_t utf16_length;
		surrogate_pair pair;
		for (auto it = sv8.cbegin(); it != sv8.cend();)
		{
			helper::codepoint::utf8_to_utf16((const char8_t*)(&(*it)), utf8_length, pair, utf16_length);
			out_u16.append(pair, pair + utf16_length);
			it += utf8_length;
		}
		out_u16.shrink_to_fit();
		return true;
	}

	OPEN_STRING_EXPORT
	inline bool convert_append(std::u16string_view sv16, std::string& out_u8)
	{
		constexpr size_t CHAR_IN_SURROGATE_PAIR = 2;
		out_u8.reserve(out_u8.size() + sv16.size() * CHAR_IN_SURROGATE_PAIR);
		size_t utf16_length;
		size_t utf8_length;
		utf8_sequence seq;
		for (auto it = sv16.cbegin(); it != sv16.cend();)
		{
			helper::codepoint::utf16_to_utf8(&*it, utf16_length, seq, utf8_length);
			out_u8.append(seq, seq + utf8_length);
			it += utf16_length;
		}
		out_u8.shrink_to_fit();
		return true;
	}

	// not used temporarily
	/*bool convert_append(std::string_view sv8, std::u32string& out_u32)
	{

	}

	bool convert_append(std::u32string_view sv32, std::string& out_u8)
	{

	}

	bool convert_append(std::u16string_view sv16, std::u32string& out_u32)
	{

	}

	bool convert_append(std::u32string_view sv32, std::u16string& out_u16)
	{

	}*/
}

_NS_OSTR_END
