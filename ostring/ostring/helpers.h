#pragma once
#include "definitions.h"
#include "char_types.h"
#include <iterator>

_NS_OSTR_BEGIN

namespace helper
{
	namespace codepoint
	{
		static_assert(sizeof(uint8_t) == sizeof(ansi_char), "different size between uint8_t and ansi char");
		constexpr uint8_t HIGHEST_MASK = 0b10000000;

		// Unicode constants
		// leading (high) surrogates: 0xd800 - 0xdbff
		// trailing (low) surrogates: 0xdc00 - 0xdfff
		constexpr uint16_t LEAD_SURROGATE_MIN =		0b1101100000000000;
		constexpr uint16_t LEAD_SURROGATE_MAX =		0b1101101111111111;
		constexpr uint16_t TRAIL_SURROGATE_MIN =	0b1101110000000000;
		constexpr uint16_t TRAIL_SURROGATE_MAX =	0b1101111111111111;

		constexpr uint32_t FIRST_PLANE_MAX =		0xffffu;
		constexpr uint32_t SURROGATE_MASK =			0b001111111111u;
		constexpr uint32_t SUPPLEMENTARY_DELTA =	1u << 16;
		constexpr uint8_t SURROGATE_LEAD_OFFSET = 10;

		constexpr uint8_t UTF8_BIT_SEQUENCE_FOLLOWING = 6;

		inline bool is_lead_surrogate(uint16_t c)
		{
			return (c >= LEAD_SURROGATE_MIN && c <= LEAD_SURROGATE_MAX);
		}

		inline bool is_trail_surrogate(uint16_t c)
		{
			return (c >= TRAIL_SURROGATE_MIN && c <= TRAIL_SURROGATE_MAX);
		}

		inline bool is_surrogate(uint16_t c)
		{
			return (c >= LEAD_SURROGATE_MIN && c <= TRAIL_SURROGATE_MAX);
		}

		inline bool is_surrogate_pair(uint16_t cl, uint16_t ct)
		{
			return is_lead_surrogate(cl) && is_trail_surrogate(ct);
		}

		inline bool is_first_plane(uint32_t value)
		{
			return value <= FIRST_PLANE_MAX;
		}

		inline bool is_supplementary_planes(uint32_t value)
		{
			return value > FIRST_PLANE_MAX;
		}

        // size of a code point in utf-8
        inline size_t utf8_sequence_length(uint8_t c, uint8_t& out_mask)
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

		inline void utf8_to_utf32(uint8_t const* const utf8, size_t& out_utf8_length, uint32_t& out_utf32)
		{
			out_utf32 = 0;
			out_utf8_length = 0; 
			if (!utf8) return;
			uint8_t c0 = utf8[0];
			if (!c0) return; 
			uint8_t utf8_mask;
			out_utf8_length = utf8_sequence_length(c0, utf8_mask);
			out_utf32 = c0 & (~utf8_mask);
			for (size_t i = 1; i < out_utf8_length; ++i)
			{
				const uint8_t ci = utf8[i];
				out_utf32 <<= UTF8_BIT_SEQUENCE_FOLLOWING;
				const size_t assert_zero = utf8_sequence_length(ci, utf8_mask);
				out_utf32 |= ci & (~utf8_mask);
			}
		}

		inline void utf16_to_utf32(uint16_t const* const utf16, size_t& out_utf16_length, uint32_t& out_utf32, endian e = endian::big)
		{
			// assume little endian temporarily
			out_utf32 = 0;
			out_utf16_length = 0;
			if (!utf16) return;
			uint16_t c0 = utf16[0];
			if (!c0) return;
			out_utf32 = c0;
			if (is_lead_surrogate(c0))
			{
				uint16_t c1 = utf16[1];
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

		inline void utf32_to_utf16(uint32_t utf32, surrogate_pair& out_utf16_char, size_t& out_utf16_length, endian e = endian::big)
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
				out_utf16_char[0] = static_cast<uint16_t>(((utf32 >> SURROGATE_LEAD_OFFSET)) + LEAD_SURROGATE_MIN);
				out_utf16_char[1] = static_cast<uint16_t>((utf32 & SURROGATE_MASK) + TRAIL_SURROGATE_MIN);
			}
			else
			{
				out_utf16_length = 1;
				out_utf16_char[0] = utf32;
			}
		}

		// convert utf8 sequence into utf16 char(s)
		// @param utf8: the utf8 sequence
		// @param out_utf8_length: length of utf8 sequence
		// @param out_utf16_char: utf16 char wich hold the real value of grapheme
		// @param out_utf16_length: array length of param out_utf16_char, 2 when surrogate pair
		inline void utf8_to_utf16(const uint8_t* utf8, size_t& out_utf8_length, surrogate_pair& out_utf16_char, size_t& out_utf16_length, endian e = endian::big)
		{
			uint32_t value;
			utf8_to_utf32(utf8, out_utf8_length, value);
			utf32_to_utf16(value, out_utf16_char, out_utf16_length, e);
		}
	}

	namespace string
	{
		// check the type is these 5 types or not
		template<typename T>
		using is_char_type_t =
			std::enable_if
			<
				std::is_same_v<T, ansi_char> ||
				std::is_same_v<T, wide_char> ||
				std::is_same_v<T, uint8_t> ||
				std::is_same_v<T, uint16_t> ||
				std::is_same_v<T, uint32_t>
			>;

		// compare two character
		template
		<
			typename T,
			typename = is_char_type_t<T>
		>
		inline int compare(T lhs, T rhs)
		{
			if (lhs < rhs) return -1;
			if (lhs > rhs) return 1;
			return 0;
		}

		// compare two array of data which are both end with '\0'
		template
		<
			typename T,
			typename = is_char_type_t<T>
		>
		inline int compare(T* lhs, T* rhs)
		{
			if (lhs == rhs) return 0;
			while (*lhs && *rhs)
			{
				const T& l = *lhs++;
				const T& r = *rhs++;
				const int value = compare(l, r);
				if (value != 0) return value;
			}
			// compare '\0' and another
			return compare(*lhs, *rhs);
		}

		// compare two array of data
		// param size should less than the size of both lhs and rhs
		template
			<
			typename T,
			typename = is_char_type_t<T>
		>
		inline int compare(T* lhs, T* rhs, size_t size)
		{
			if (lhs == rhs) return 0;
			size_t cnt = 0;
			while (cnt < size)
			{
				const T& l = *lhs++;
				const T& r = *rhs++;
				const int value = compare(l, r);
				if (value != 0) return value;
				++cnt;
			}
			return 0;
		}
	}

}

_NS_OSTR_END
