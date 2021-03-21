#pragma once
#include "definitions.h"
#include "char_types.h"
#include <vector>
#include "char_types.h"
#include "helpers.h"
#include <cstdarg>

_NS_OSTR_BEGIN

enum class encoding : char8_t
{
	ansi,
	utf8
};

class string
{
public:

	string() = default;
	string(string&&) = default;
	string(const string&) = default;
	string& operator=(string&&) = default;
	string& operator=(const string&) = default;

	// Initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of characters,
	// and how it's encoded.
	// @param src: the c-style char.
	// @param ec: how it's encoded.
	string(const ansi_char* src, encoding ec = encoding::ansi)
		:_surrogate_pair_count(0)
	{
		size_t len = strlen(src);
		_wa.reserve(len + 1);

		switch (ec)
		{
		case encoding::ansi:
		{
			// ansi is a subset of the first plane
			_wa.insert(_wa.cbegin(), src, src + len);
			break;
		}
		case encoding::utf8:
		{
			size_t utf8_length;
			size_t utf16_length;
			surrogate_pair pair;
			const ansi_char* end = src + len;
			while (src < end)
			{
				helper::codepoint::utf8_to_utf16((const char8_t*)src, utf8_length, pair, utf16_length);
				_wa.insert(_wa.cend(), pair, pair + utf16_length);
				if (utf16_length == 2) ++_surrogate_pair_count;
				src += utf8_length;
			}
			break;
		}
		default:
			"not supported encoding type when sending in ansi.";
			break;
		}
		append_terminal();
	}

	// Initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of wide characters,
	// and the endian it is.
	// @param src: the c-style wide char.
	// @param ec: what's the endian it is.
	string(const wide_char* src, endian e = endian::big)
		:_surrogate_pair_count(0)
	{
		// assume big endian temporarily
		size_t len = wcslen(src) + 1;
		_wa.reserve(len);
		_wa.insert(_wa.cend(), src, src + len); 
		calculate_surrogate();
	}

	// Initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of char16_t characters,
	// and the endian it is.
	// @param src: the c-style char16_t string.
	// @param ec: what's the endian it is.
	string(const char16_t* src, endian e = endian::big)
		: string((const wide_char*)src, e)
	{}

	// Initializes a new instance of the string class with multi count of specific char.
	// cccc..[count]..cccc
	// @param c: the char used to init.
	// @param count: how may c.
	string(const wide_char c, size_t count = 1)
		:_surrogate_pair_count(0)
	{
		// ansi as well as BMP in first plane can trans to wide char without side effect
		_wa.resize(count, c); 
		append_terminal();
	}

	// Initializes a new instance of the string class with multi count of specific string.
	// strstrstr..[count]..strstr
	// @param str: the string used to init.
	// @param count: how may str.
	string(const string& str, size_t count)
	{
		_wa.reserve(str.length() * count + 1);
		for (size_t i = 0; i < count; ++i)
			operator+=(str);
	}

	// @return: the length of string.
	size_t length() const
	{
		const size_t length_with_0 = _wa.size() - _surrogate_pair_count;
		return length_with_0 > 0 ? length_with_0 - 1 : length_with_0;
	}

	// @return: whether this string is empty.
	bool is_empty() const
	{
		return length() == 0;
	}

	// Compare two strings.
	// @param rhs: another string.
	// @return:
	// return 1 if this is greater than rhs.
	// return 0 if this is equal to rhs.
	// return -1 if this is less than rhs.
	int string_compare(const string& rhs) const
	{
		const size_t lhs_length = length();
		const size_t rhs_length = rhs.length();
		if (lhs_length == 0 && rhs_length == 0) return 0;
		const int size_compare = helper::string::literal_compare(lhs_length, rhs_length);
		if (size_compare != 0) return size_compare;
		const int cmp = helper::string::string_compare(_wa.data(), rhs._wa.data(), lhs_length);
		return cmp;
	}

	// Are they totally equal?
	// @param rhs: another string.
	// @return: true if totally equal.
	bool operator==(const string& rhs) const
	{
		return string_compare(rhs) == 0;
	}

	// Are they different?
	// @param rhs: another string.
	// @return: true if different.
	bool operator!=(const string& rhs) const
	{
		return string_compare(rhs) != 0;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than rhs.
	bool operator<(const string& rhs) const
	{
		return string_compare(rhs) < 0;
	}
	
	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than or equal to rhs.
	bool operator<=(const string& rhs) const
	{
		return string_compare(rhs) <= 0;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater thsn rhs.
	bool operator>(const string& rhs) const
	{
		return string_compare(rhs) > 0;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater than or equal to rhs.
	bool operator>=(const string& rhs) const
	{
		return string_compare(rhs) >= 0;
	}

	// Append back.
	// string("this") + "rhs" == string("thisrhs")
	// @param rhs: append rhs back this string.
	// @return: ref this string.
	string& operator+=(const string& rhs)
	{
		auto it_target = _wa.cend() - (_wa.size() ? 1 : 0);
		_wa.insert(it_target, rhs._wa.cbegin(), rhs._wa.cend() - 1);
		_surrogate_pair_count += rhs._surrogate_pair_count; 
		append_terminal();
		return *this;
	}

	// Append back, get a new string instance without modify this string.
	// string("this") + "rhs" == string("thisrhs")
	// @param rhs: append rhs back this string.
	// @return: a new result string instance.
	string operator+(const string& rhs)
	{
		string str = *this;
		str += rhs;
		return str;
	}

	// Append back.
	// @param rhs: append rhs back this string.
	// @return: ref this string.
	template<typename T>
	string& operator+=(T rhs)
	{
		operator+=(to_string(rhs));
		return *this;
	}

	// Append back, get a new string instance without modify this string.
	// @param rhs: append rhs back this string.
	// @return: a new result string instance.
	template<typename T>
	string operator+(T rhs)
	{
		string str = *this;
		str += rhs;
		return str;
	}

	// Get a new substring from specific position with specific size
	// string("abcdefg").substring(2, 3) == string("cde");
	// @param from: from where to start, 0 if from begin.
	// @param size: how many chars you want.
	// @return: the new substring instance
	string substring(size_t from, size_t size = SIZE_MAX) const
	{
		string ret;
		const size_t uint16_size = std::min(size, length() - from);
		if (_surrogate_pair_count == 0)
		{
			ret._wa.reserve(uint16_size + 1);
			ret._wa.insert(ret._wa.cbegin(), _wa.cbegin() + from, _wa.cbegin() + (from + uint16_size));
		}
		else
		{
			auto from_it = helper::string::codepoint_count(_wa.cbegin(), from);

			const size_t substr_surrogate_pair_count = _surrogate_pair_count - ((from_it - _wa.cbegin()) - from);
			const size_t real_size = uint16_size + substr_surrogate_pair_count;
			ret._wa.reserve(real_size + 1);
			ret._wa.insert(ret._wa.cbegin(), from_it, from_it + real_size);
			ret._surrogate_pair_count = substr_surrogate_pair_count;
		}
		ret.append_terminal();
		return ret;
	}

	// Get the index of specific string
	// string("abcdefg").index_of("cde") == 2;
	// @param substr: substring to search.
	// @param from: from where to search.
	// @param length: how many length in this string to search, that means, searching from "from" to "from + length".
	// @return: the new substring instance
	size_t index_of(const string& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		const size_t index = position_codepoint_to_index(from);
		const size_t real_size = std::min(length, _wa.size() - index);
		const size_t index_found = helper::string::string_search(_wa.data() + index, real_size, substr._wa.data(), substr.length(), cs);
		if (index_found == SIZE_MAX) return SIZE_MAX;
		return position_index_to_codepoint(index_found + index);
	}

	void replace(size_t from, size_t count, const string& dest, case_sensitivity cs = case_sensitivity::sensitive)
	{
		size_t len = _wa.size();

		from = position_codepoint_to_index(from);

		const int size_delta = std::max(1, (int)dest._wa.size()) - std::max<int>(1, (int)count + 1);
		helper::vector::adjust_size(_wa, from, size_delta);
		for (size_t i = 0; i < dest._wa.size() - 1; ++i)
			_wa[from + i] = dest._wa[i];

		calculate_surrogate();
	}

	size_t replace(const string& src, const string& dest, case_sensitivity cs = case_sensitivity::sensitive)
	{
		// src should NOT be empty!
		if (src.is_empty()) return SIZE_MAX; // ASSERT!

		size_t len = _wa.size();
		size_t index = 0;
		index = index_of(src, index, SIZE_MAX, cs);

		size_t cnt = 0;
		while (index < len)
		{
			const int size_delta = std::max(1, (int)dest._wa.size()) - std::max(1, (int)src._wa.size());
			helper::vector::adjust_size(_wa, index, size_delta);
			for (size_t i = 0; i < dest._wa.size() - 1; ++i)
				_wa[index + i] = dest._wa[i];
			// split into two steps beware of negative number when use type size_t
			_surrogate_pair_count += dest._surrogate_pair_count;
			_surrogate_pair_count -= src._surrogate_pair_count;
			++cnt;
			index += src._wa.size() - 1;
			index = index_of(src, index, SIZE_MAX, cs);
		}
		return cnt;
	}

	// Returns a new string in which all occurrences of a specified string in the current instance
	// are replaced with another specified string.
	// @return: how many substrings have been replaced
	string replace_new(const string& src, const string& dest, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		string new_inst(*this);
		new_inst.replace(src, dest, cs);
		return new_inst;
	}

	template<typename...Args>
	static string format(const string& fmt, Args...args)
	{
		return format(fmt.to_utf16(), args...);
	}

	template<typename T, typename...Args, typename = helper::character::is_char_type_t<T>>
	static string format(T* fmt, Args...args)
	{
		constexpr size_t n = sizeof...(Args);
		string argstrings[n];

		args_to_strings(argstrings, args...);

		string str(fmt);

		size_t index_left = str.index_of("{");
		while (index_left < str._wa.size())
		{
			size_t index_right = str.index_of("}", index_left);
			size_t index_placeholder = helper::string::to_uint(str._wa.data() + index_left + 1, index_right - index_left - 1);

			str.replace(index_left, index_right - index_left + 1, argstrings[index_placeholder]);

			index_left = str.index_of("{", index_left);
		}

		return str;
	}

	const char16_t* to_utf16() const
	{
		return _wa.data();
	}

private:

	void calculate_surrogate()
	{
		_surrogate_pair_count = helper::string::count_surrogate_pair(_wa.cbegin(), _wa.cend());
	}

	void append_terminal()
	{
		if (_wa.empty() || _wa.back() != 0)
			_wa.push_back(0);
	}

	size_t position_codepoint_to_index(size_t codepoint_count) const
	{
		auto from_it = helper::string::codepoint_count(_wa.cbegin(), codepoint_count);
		return from_it - _wa.cbegin();
	}

	size_t position_index_to_codepoint(size_t index) const
	{
		return index - helper::string::count_surrogate_pair(_wa.cbegin(), _wa.cbegin() + index);
	}


	static inline ostr::string from_std_to_ostr(const std::string& std_str)
	{
		return std_str.c_str();
	}

	static inline ostr::string from_std_to_ostr(const std::wstring& std_wstr)
	{
		return std_wstr.c_str();
	}

	// use std converter temporily
	// use std converter temporily
	// use std converter temporily

	template<typename T>
	static inline ostr::string to_string(const T& variable)
	{
		return variable.to_string();
	}

	template<>
	static inline ostr::string to_string(const char* const & str_variable)
	{
		return from_std_to_ostr(std::string(str_variable));
	}

	template<>
	static inline ostr::string to_string(const int& int_variable)
	{
		return from_std_to_ostr(std::to_wstring(int_variable));
	}

	template<>
	static inline ostr::string to_string(const float& float_variable)
	{
		return from_std_to_ostr(std::to_wstring(float_variable));
	}

	template<>
	static inline ostr::string to_string(const double& double_variable)
	{
		return from_std_to_ostr(std::to_wstring(double_variable));
	}

	template<typename...Args>
	using FixedStringArray = string[sizeof...(Args)];

	template<typename T>
	static void args_to_strings(string* p_str, T t)
	{
		*p_str = to_string(t);
	}

	template<typename T, typename...Args>
	static void args_to_strings(string* p_str, T t, Args...args)
	{
		*p_str = to_string(t);
		args_to_strings(p_str + 1, args...);
	}

private:

	std::vector<char16_t> _wa;	// wchar_t array, NOT ends with '\0'

	size_t _surrogate_pair_count = 0;

};

_NS_OSTR_END
