#pragma once
#include "definitions.h"
#include "char_types.h"
#include <iterator>
#include <string>
#include <vector>
#include <functional>

_NS_OSTR_BEGIN

namespace helper
{
	namespace codepoint
	{
		constexpr char8_t HIGHEST_MASK = 0b10000000;

		// Unicode constants
		// leading (high) surrogates: 0xd800 - 0xdbff
		// trailing (low) surrogates: 0xdc00 - 0xdfff
		constexpr char16_t LEAD_SURROGATE_MIN =		0b1101100000000000;
		constexpr char16_t LEAD_SURROGATE_MAX =		0b1101101111111111;
		constexpr char16_t TRAIL_SURROGATE_MIN =	0b1101110000000000;
		constexpr char16_t TRAIL_SURROGATE_MAX =	0b1101111111111111;

		constexpr char32_t FIRST_PLANE_MAX =		0xffffu;
		constexpr char32_t SURROGATE_MASK =			0b001111111111u;
		constexpr char32_t SUPPLEMENTARY_DELTA =	1u << 16;
		constexpr char8_t SURROGATE_LEAD_OFFSET = 10;

		constexpr char8_t UTF8_BIT_SEQUENCE_FOLLOWING = 6;

		inline bool is_lead_surrogate(char16_t c)
		{
			return (c >= LEAD_SURROGATE_MIN && c <= LEAD_SURROGATE_MAX);
		}

		inline bool is_trail_surrogate(char16_t c)
		{
			return (c >= TRAIL_SURROGATE_MIN && c <= TRAIL_SURROGATE_MAX);
		}

		inline bool is_surrogate(char16_t c)
		{
			return (c >= LEAD_SURROGATE_MIN && c <= TRAIL_SURROGATE_MAX);
		}

		inline bool is_surrogate_pair(char16_t cl, char16_t ct)
		{
			return is_lead_surrogate(cl) && is_trail_surrogate(ct);
		}

		inline bool is_first_plane(char32_t value)
		{
			return value <= FIRST_PLANE_MAX;
		}

		inline bool is_supplementary_planes(char32_t value)
		{
			return value > FIRST_PLANE_MAX;
		}

        // size of a code point in utf-8
        inline size_t utf8_sequence_length(char8_t c, char8_t& out_mask)
        {
			out_mask = 0;
			size_t size = 0;
			while (c & HIGHEST_MASK)
			{
				++size;
				out_mask >>= 1;
				out_mask |= HIGHEST_MASK;
				c <<= 1;
			}
			// 1 byte when start with 0
			// 0 byte when start with 10
			// 2 bytes when start with 110
			// n bytes when start with 1..(n)..0
			return size > 1 ? size : 1 - size;
        }

		inline void utf8_to_utf32(char8_t const* const utf8, size_t& out_utf8_length, char32_t& out_utf32)
		{
			out_utf32 = 0;
			out_utf8_length = 0; 
			if (!utf8) return;
			char8_t c0 = utf8[0];
			if (!c0) return; 
			char8_t utf8_mask;
			out_utf8_length = utf8_sequence_length(c0, utf8_mask);
			out_utf32 = c0 & (~utf8_mask);
			for (size_t i = 1; i < out_utf8_length; ++i)
			{
				const char8_t ci = utf8[i];
				out_utf32 <<= UTF8_BIT_SEQUENCE_FOLLOWING;
				const size_t assert_zero = utf8_sequence_length(ci, utf8_mask);
				out_utf32 |= ci & (~utf8_mask);
			}
		}

		inline void utf16_to_utf32(char16_t const* const utf16, size_t& out_utf16_length, char32_t& out_utf32, endian e = endian::big)
		{
			// assume little endian temporarily
			out_utf32 = 0;
			out_utf16_length = 0;
			if (!utf16) return;
			char16_t c0 = utf16[0];
			if (!c0) return;
			out_utf32 = c0;
			if (is_lead_surrogate(c0))
			{
				char16_t c1 = utf16[1];
				const bool assert_true = is_trail_surrogate(c1);
				if (assert_true)
				{
					out_utf16_length = 2;
					out_utf32 &= (~LEAD_SURROGATE_MIN);
					out_utf32 <<= SURROGATE_LEAD_OFFSET;
					out_utf32 |= (c1 & (~TRAIL_SURROGATE_MIN));
					out_utf32 += SUPPLEMENTARY_DELTA;
				}
				else 
				{
					// should we warn out something?
					// or keep this field for some customization?
				}
			}
			else
			{
				out_utf16_length = 1;
			}
		}

		inline void utf32_to_utf16(char32_t utf32, surrogate_pair& out_utf16_char, size_t& out_utf16_length, endian e = endian::big)
		{
			// assume little endian temporarily
			if (!utf32) return;

			out_utf16_char[0] = 0;
			out_utf16_char[1] = 0;
			out_utf16_length = 0;

			// make a surrogate pair
			if (is_supplementary_planes(utf32))
			{
				out_utf16_length = 2;
				out_utf16_char[0] = static_cast<char16_t>(((utf32 >> SURROGATE_LEAD_OFFSET)) + LEAD_SURROGATE_MIN);
				out_utf16_char[1] = static_cast<char16_t>((utf32 & SURROGATE_MASK) + TRAIL_SURROGATE_MIN);
			}
			else
			{
				out_utf16_length = 1;
				out_utf16_char[0] = static_cast<char16_t>( utf32 );
			}
		}

		// convert utf8 sequence into utf16 char(s)
		// @param utf8: the utf8 sequence
		// @param out_utf8_length: length of utf8 sequence
		// @param out_utf16_char: utf16 char wich hold the real value of grapheme
		// @param out_utf16_length: array length of param out_utf16_char, 2 when surrogate pair
		inline void utf8_to_utf16(const char8_t* utf8, size_t& out_utf8_length, surrogate_pair& out_utf16_char, size_t& out_utf16_length, endian e = endian::big)
		{
			char32_t value;
			utf8_to_utf32(utf8, out_utf8_length, value);
			utf32_to_utf16(value, out_utf16_char, out_utf16_length, e);
		}
	}

	namespace character
	{
		template<typename T>
		inline T char_lowercase(T c)
		{
			if (c <= 'Z' && c >= 'A')
				return c - ('A' - 'a');
			return c;
		}

		template<typename T>
		inline auto& case_folder(case_sensitivity cs)
		{
			static const std::function<T(T)> _origin([](T c) {return c; });
			static const std::function<T(T)> _lowercase([](T c) {return character::char_lowercase(c); });
			return
				(cs == case_sensitivity::sensitive) ? _origin : _lowercase;
		}

		template<typename T>
		inline auto& case_predicate(case_sensitivity cs)
		{
			static const std::function<bool(T, T)> _origin([](T lhs, T rhs) {return lhs == rhs; });
			static const std::function<bool(T, T)> _lowercase([](T lhs, T rhs) {return character::char_lowercase(lhs) == character::char_lowercase(rhs); });
			return
				(cs == case_sensitivity::sensitive) ? _origin : _lowercase;
		}

		template<typename T>
		inline bool is_number(T c)
		{
			return c >= '0' && c <= '9';
		}

		template<typename T>
		inline char8_t to_number(T c)
		{
			if (is_number(c)) return c - '0';
			return CHAR_MAX;
		}

	}

	namespace string
	{

		template<typename T>
		inline size_t to_uint(T* str, size_t len)
		{
			size_t ans = 0;
			for (T* c = str; c < str + len; ++c)
			{
				if (character::is_number(*c))
					ans = ans * 10 + character::to_number(*c);
				else
					return ans;
			}
			return ans;
		}

		// calculate surrogate pair inside, only work for char16_t
		template<typename _Iter, typename = ::std::enable_if<::std::is_same_v<::std::iterator_traits<_Iter>, char16_t>>>
		inline size_t count_surrogate_pair(_Iter from, _Iter end)
		{
			size_t surrogate_pair_count = 0;
			while (from < end)
			{
				if (from < (end - 1) && helper::codepoint::is_surrogate_pair(from[0], from[1]))
				{
					++surrogate_pair_count;
					++from;
				}
				++from;
			}
			return surrogate_pair_count;
		}

		// calculate surrogate pair inside, only work for char16_t
		template<typename _Iter, typename = std::enable_if<std::is_same_v<std::iterator_traits<_Iter>, char16_t>>>
		inline _Iter codepoint_count_to_iterator(_Iter from, size_t count, _Iter end)
		{
			while (count > 0 && from != end)
			{
				if (((end - from) > 1) && helper::codepoint::is_surrogate_pair(from[0], from[1]))
					++from;
				--count;
				++from;
			}
			return from;
		}

		// calculate surrogate pair from end inside, only work for char16_t
		template<typename _Iter, typename = std::enable_if<std::is_same_v<std::iterator_traits<_Iter>, char16_t>>>
		inline _Iter codepoint_count_to_iterator_backward(_Iter rfrom, size_t count, _Iter rend)
		{
			while (count > 0 && rfrom != rend)
			{
				if (((rend - rfrom) > 1) && helper::codepoint::is_surrogate_pair(rfrom[1], rfrom[0]))
					++rfrom;
				--count;
				++rfrom;
			}
			return rfrom;
		}

	}
}

_NS_OSTR_END
