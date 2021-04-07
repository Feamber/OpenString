#pragma once
#include <iterator>
#include <string>
#include <vector>
#include <functional>
#include "definitions.h"
#include "types.h"

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
        inline small_size_t utf8_sequence_length(char8_t c, char8_t& out_mask)
        {
			out_mask = 0;
			small_size_t size = 0;
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

		inline void utf8_to_utf32(char8_t const* const utf8, small_size_t& out_utf8_length, char32_t& out_utf32)
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
				const size_t assert_zero = utf8_sequence_length(ci, utf8_mask); (void)assert_zero;
				out_utf32 |= ci & (~utf8_mask);
			}
		}

		inline void utf32_to_utf8(char32_t const utf32, utf8_sequence& out_utf8, small_size_t& out_utf8_length)
		{
			if (utf32 < 0x80)                        // one octet
			{
				out_utf8_length = 1;
				out_utf8[0] = static_cast<uint8_t>(utf32);
			}
			else if (utf32 < 0x800) {                // two octets
				out_utf8_length = 2;
				out_utf8[0] = static_cast<uint8_t>((utf32 >> 6) | 0xc0);
				out_utf8[1] = static_cast<uint8_t>((utf32 & 0x3f) | 0x80);
			}
			else if (utf32 < 0x10000) {              // three octets
				out_utf8_length = 3;
				out_utf8[0] = static_cast<uint8_t>((utf32 >> 12) | 0xe0);
				out_utf8[1] = static_cast<uint8_t>(((utf32 >> 6) & 0x3f) | 0x80);
				out_utf8[2] = static_cast<uint8_t>((utf32 & 0x3f) | 0x80);
			}
			else {                                // four octets
				out_utf8_length = 4;
				out_utf8[0] = static_cast<uint8_t>((utf32 >> 18) | 0xf0);
				out_utf8[1] = static_cast<uint8_t>(((utf32 >> 12) & 0x3f) | 0x80);
				out_utf8[2] = static_cast<uint8_t>(((utf32 >> 6) & 0x3f) | 0x80);
				out_utf8[3] = static_cast<uint8_t>((utf32 & 0x3f) | 0x80);
			}
		}

		inline void utf16_to_utf32(char16_t const* const utf16, small_size_t& out_utf16_length, char32_t& out_utf32, endian e = endian::big)
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

		inline void utf32_to_utf16(char32_t utf32, surrogate_pair& out_utf16_char, small_size_t& out_utf16_length, endian e = endian::big)
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
		inline void utf8_to_utf16(const char8_t* utf8, small_size_t& out_utf8_length, surrogate_pair& out_utf16_char, small_size_t& out_utf16_length, endian e = endian::big)
		{
			char32_t value;
			utf8_to_utf32(utf8, out_utf8_length, value);
			utf32_to_utf16(value, out_utf16_char, out_utf16_length, e);
		}

		// convert utf16 char(s) into utf8 sequence
		// @param utf8: the utf8 sequence
		// @param out_utf8_length: length of utf8 sequence
		// @param out_utf16_char: utf16 char wich hold the real value of grapheme
		// @param out_utf16_length: array length of param out_utf16_char, 2 when surrogate pair
		inline void utf16_to_utf8(char16_t const* const utf16, small_size_t& out_utf16_length, utf8_sequence& out_utf8, small_size_t& out_utf8_length, endian e = endian::big)
		{
			char32_t value;
			utf16_to_utf32(utf16, out_utf16_length, value, e);
			utf32_to_utf8(value, out_utf8, out_utf8_length);
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

	namespace hash
	{
		// CRC32 Table (zlib polynomial)
		static constexpr uint32_t crc_table[256] = 
		{
			0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
			0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
			0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
			0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
			0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
			0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
			0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
			0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
			0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
			0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
			0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
			0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
			0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
			0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
			0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
			0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
			0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
			0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
			0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
			0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
			0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
			0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
			0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
			0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
			0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
			0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
			0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
			0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
			0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
			0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
			0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
			0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
			0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
			0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
			0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
			0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
			0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
			0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
			0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
			0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
			0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
			0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
			0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
		};

		/*template<typename T>
		constexpr uint32_t __crc32(std::basic_string_view<T> str)
		{
			return (str == std::basic_string_view<T>())
				? 0xFFFFFFFF
				: ((__crc32(str.substr(0, str.size() - 1)) >> 8) ^ crc_table[(__crc32(str.substr(0, str.size() - 1)) ^ str[str.size() - 1]) & 0x000000FF]);
		}*/

		template<typename T>
		constexpr uint32_t __crc32(std::basic_string_view<T> str)
		{
			uint32_t ans = 0xFFFFFFFF;
			for (size_t i = 0; i < str.size(); ++i)
			{
				ans = ((ans >> 8) ^ crc_table[(ans ^ str[i]) & 0x000000FF]);
			}
			return ans;
		}

		template<typename T>
		constexpr uint32_t hash_crc32(std::basic_string_view<T> str)
		{
			return __crc32<T>(str) ^ 0xFFFFFFFF;
		}
	}
}

_NS_OSTR_END
