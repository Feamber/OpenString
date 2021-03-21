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
		static_assert(sizeof(char8_t) == sizeof(ansi_char), "different size between char8_t and ansi char");
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
		// check the type is these 5 types or not
		template<typename T>
		using is_char_type_t =
			std::enable_if
			<
			std::is_same_v<T, ansi_char> ||
			std::is_same_v<T, wide_char> ||
			std::is_same_v<T, char8_t> ||
			std::is_same_v<T, char16_t> ||
			std::is_same_v<T, char32_t>
			>;

		template<typename T, typename = is_char_type_t<T>>
		inline T char_lowercase(T c)
		{
			if (c <= 'Z' && c >= 'A')
				return c - ('A' - 'a');
			return c;
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

		template<typename T, typename = character::is_char_type_t<T>>
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
		template<typename _Iter, typename = std::enable_if<std::is_same_v<std::iterator_traits<_Iter>, char16_t>>>
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
		inline _Iter codepoint_count(_Iter from, size_t count)
		{
			while (count > 0 && *from)
			{
				if (helper::codepoint::is_surrogate_pair(from[0], from[1]))
					++from;
				--count;
				++from;
			}
			return from;
		}

		template<typename T>
		inline auto& case_folder(case_sensitivity cs)
		{
			static const std::function<T(T)> _origin([](T c) {return c; });
			static const std::function<T(T)> _lowercase([](T c) {return character::char_lowercase(c); });
			return 
				(cs == case_sensitivity::sensitive) ? _origin : _lowercase;
		}

		// compare two character
		template<typename T, typename = std::enable_if_t<std::is_literal_type_v<T>>>
		inline int literal_compare(T lhs, T rhs, case_sensitivity cs = case_sensitivity::sensitive)
		{
			lhs = case_folder<T>(cs)(lhs);
			rhs = case_folder<T>(cs)(rhs);
			if (lhs < rhs) return -1;
			if (lhs > rhs) return 1;
			return 0;
		}

		// compare two array of data which are both end with '\0'
		template<typename T, typename = character::is_char_type_t<T>>
		inline int string_compare(T* lhs, T* rhs, size_t size = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive)
		{
			if (lhs == rhs) return 0;
			size_t cnt = 0;
			while (cnt++ < size)
			{
				if (!*lhs || !*rhs)
					// compare '\0' and another
					return literal_compare(*lhs, *rhs, cs);

				const int value = literal_compare(*lhs++, *rhs++, cs);
				if (value != 0) return value;
			}
			return 0;
		}

		// Find a character from src , return the index in src.
		// Return SIZE_MAX if not found
		// @param src: where to search.
		// @param c: what to search.
		// @param src_size: how long in src to search, SIZE_MAX if search until meet '\0'. This is the count of real char in memory, that means, count of a surrogate pair is 2, not 1.
		// @param cs: is this case sensitivity.
		// @ return: return the index if found, or SIZE_MAX if not.
		template<typename T>
		inline size_t char_search(T* src, T c, size_t src_size = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive)
		{
			std::remove_const_t<T> current = case_folder<T>(cs)(c);

			T* src_it = src;

			while (src_size > 0)
			{
				std::remove_const_t<T> src_char = *src_it;

				if (cs == case_sensitivity::insensitive)
					src_char = character::char_lowercase(src_char);

				if (src_char == 0)
					return SIZE_MAX;
				if (src_char == current)
					return src_it - src;

				++src_it;
			}

			return SIZE_MAX;
		}


		// Find substr from src with Boyer-Moore algprithm, return the index in src.
		// Return SIZE_MAX if not found
		// @param haystack: where to search.
		// @param haystack_size: how long in src to search. This is the count of real char in memory, that means, count of a surrogate pair is 2, not 1.
		// @param needle: what to search.
		// @param needle_size: how long in substr to be searched. This is the count of real char in memory, that means, count of a surrogate pair is 2, not 1.
		// @param cs: is this case sensitivity.
		// @return: index in src, SIZE_MAX if not found.
		template<typename T>
		size_t string_search_bm(T* haystack, size_t haystack_size, T* needle, size_t needle_size, case_sensitivity cs = case_sensitivity::sensitive)
		{
			const size_t l = haystack_size;
			const size_t pl = needle_size;

			if (pl == 0)
				return 0;

			std::remove_const_t<T> skiptable[256]{ 0 };

			// init skiptable
			{
				T* needle1 = needle;
				const size_t len = needle_size;
				size_t l = std::min(len, size_t(255));
				memset(skiptable, static_cast<T>(l), 256 * sizeof(T));
				needle1 += len - l;
				if (true) 
				{
					while (l--) 
					{
						skiptable[*needle1 & 0xff] = std::remove_const_t<T>(l);
						++needle1;
					}
				}
			}

			const size_t pl_minus_one = pl - 1;

			T* current = haystack + pl_minus_one;
			T* end = haystack + l;
			
			while (current < end) {
				size_t skip = skiptable[case_folder<T>(cs)(*current) & 0xff];
				if (!skip) {
					// possible match
					while (skip < pl) {
						if (case_folder<T>(cs) (*(current - skip)) != case_folder<T>(cs)(needle[pl_minus_one - skip]))
							break;
						++skip;
					}
					if (skip > pl_minus_one) // we have a match
						return (current - haystack) - pl_minus_one;

					// in case we don't have a match we are a bit inefficient as we only skip by one
					// when we have the non matching char in the string.
					if (skiptable[case_folder<T>(cs) (*(current - skip)) & 0xff] == pl)
						skip = pl - skip;
					else
						skip = 1;
				}
				if (current > end - skip)
					break;
				current += skip;
			}
			
			return SIZE_MAX; // not found
		}

		// Find substr from src with hash algprithm, return the index in src.
		// Return SIZE_MAX if not found
		// @param src: where to search.
		// @param src_size: how long in src to search. This is the count of real char in memory, that means, count of a surrogate pair is 2, not 1.
		// @param substr: what to search.
		// @param substr_size: how long in substr to be searched. This is the count of real char in memory, that means, count of a surrogate pair is 2, not 1.
		// @param cs: is this case sensitivity.
		// @return: index in src, SIZE_MAX if not found.
		template<typename T>
		inline size_t string_search_hash(T* src, size_t src_size, T* substr, size_t substr_size, case_sensitivity cs = case_sensitivity::sensitive)
		{
			T* end = src + (src_size - substr_size);
			T* start = src;
			size_t hash_needle = 0;
			size_t hash_haystack = 0;

			for (size_t i = 0; i < substr_size; ++i)
			{
				hash_needle = ((hash_needle << 1) + case_folder<T>(cs)(substr[i]));
				hash_haystack = ((hash_haystack << 1) + case_folder<T>(cs)(src[i]));
			}
			const size_t ss_minus_1 = substr_size - 1;

			hash_haystack -= case_folder<T>(cs)(src[ss_minus_1]);

			while (src <= end) 
			{
				hash_haystack += case_folder<T>(cs)(src[ss_minus_1]);

				if (hash_haystack == hash_needle && string_compare(substr, src, substr_size, cs) == 0)
					return src - start;

				if ((ss_minus_1) < sizeof(std::size_t) * CHAR_BIT)
					hash_haystack -= ((size_t(case_folder<T>(cs)(*src))) << (ss_minus_1));

				hash_haystack <<= 1;
				++src;
			}

			return SIZE_MAX;
		}

		// Find substr from src, use hash algorithm when find short one in a short one, Boyer-Moore otherwise, return the index in src.
		// Return SIZE_MAX if not found
		// @param src: where to search.
		// @param src_size: how long in src to search. This is the count of real char in memory, that means, count of a surrogate pair is 2, not 1.
		// @param substr: what to search.
		// @param substr_size: how long in substr to be searched. This is the count of real char in memory, that means, count of a surrogate pair is 2, not 1.
		// @param cs: is this case sensitivity.
		// @return: index in src, SIZE_MAX if not found.
		template<typename T>
		inline size_t string_search(T* src, size_t src_size, T* substr, size_t substr_size, case_sensitivity cs = case_sensitivity::sensitive)
		{
			if (substr_size > src_size)
				return -1;
			if (!substr_size)
				return 0;
			if (!src_size)
				return -1;

			if (substr_size == 1)
				return char_search<T>(src, substr[0], src_size, cs);

			/*
				We use the Boyer-Moore algorithm in cases where the overhead
				for the skip table should pay off, otherwise we use a simple
				hash function.
			*/
			if (src_size > 500 && substr_size > 5)
				return string_search_bm(src, src_size, substr, substr_size, cs);

			/*
				We use some hashing for efficiency's sake. Instead of
				comparing strings, we compare the hash value of str with that
				of a part of this QString. Only if that matches, we call
				qt_string_compare().
			*/
			return string_search_hash(src, src_size, substr, substr_size, cs);
		}

	}

	namespace vector
	{
		template<typename T>
		void adjust_size(std::vector<T>& vec, size_t where, int size_delta)
		{
			if (size_delta > 0)
				vec.insert(vec.cbegin() + where, size_delta, 0);
			else if (size_delta < 0)
				vec.erase(vec.cbegin() + where, vec.cbegin() + where - size_delta);
		}
	}

}

_NS_OSTR_END
