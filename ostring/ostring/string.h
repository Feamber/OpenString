#pragma once
#include "definitions.h"
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include "helpers.h"

#include "../fmt/include/fmt/format.h"
#pragma comment(lib, "fmt.lib")

_NS_OSTR_BEGIN

enum class encoding : char8_t
{
	ansi,
	utf8
};


class string
{
	friend class string_view;
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
	string(const char* src, size_t length = SIZE_MAX, encoding ec = encoding::ansi)
		:_surrogate_pair_count(0)
	{
		size_t len = std::min(length, strlen(src));

		switch (ec)
		{
		case encoding::ansi:
		{
			// ansi is a subset of the first plane
			//_str.insert(_str.cbegin(), src, src + len);
			_str = std::wstring(src, src + len);
			break;
		}
		case encoding::utf8:
		{
			size_t utf8_length;
			size_t utf16_length;
			surrogate_pair pair;
			const char* end = src + len;
			while (src < end)
			{
				helper::codepoint::utf8_to_utf16((const char8_t*)src, utf8_length, pair, utf16_length);
				_str.insert(_str.cend(), pair, pair + utf16_length);
				if (utf16_length == 2) ++_surrogate_pair_count;
				src += utf8_length;
			}
			break;
		}
		default:
			"not supported encoding type when sending in ansi.";
			break;
		}
	}

	// Initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of wide characters,
	// and the endian it is.
	// @param src: the c-style wide char.
	// @param ec: what's the endian it is.
	string(const wchar_t* src, size_t length = SIZE_MAX)
		:_surrogate_pair_count(0)
		, _str(src, 0, length)
	{
		calculate_surrogate();
	}

	// Initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of char16_t characters,
	// and the endian it is.
	// @param src: the c-style char16_t string.
	// @param ec: what's the endian it is.
	string(const char16_t* src)
		: string((const wchar_t*)src)
	{}

	// Initializes a new instance of the string class with multi count of specific char.
	// cccc..[count]..cccc
	// @param c: the char used to init.
	// @param count: how may c.
	string(const wchar_t c, size_t count = 1)
		:_surrogate_pair_count(0)
	{
		// ansi as well as BMP in first plane can trans to wide char without side effect
		_str.resize(count, c);
	}

	// Initializes a new instance of the string class with std::wstring.
	// @param str: the wstring used to init.
	// @param count: how may c.
	string(const std::wstring& str)
		: _str(str)
		, _surrogate_pair_count(0)
	{
		calculate_surrogate();
	}

	// Initializes a new instance of the string class with std::string.
	// @param str: the string used to init.
	// @param count: how may c.
	string(const std::string& str)
		: string(str.c_str())
	{}

	// @return: the length of string.
	size_t length() const
	{
		return _str.size() - _surrogate_pair_count;
	}

	// @return: whether this string is empty.
	bool is_empty() const
	{
		return length() == 0;
	}

	// Are they totally equal?
	// @param rhs: another string.
	// @return: true if totally equal.
	bool operator==(const string& rhs) const
	{
		return _str == rhs._str;
	}

	// Are they different?
	// @param rhs: another string.
	// @return: true if different.
	bool operator!=(const string& rhs) const
	{
		return _str != rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than rhs.
	bool operator<(const string& rhs) const
	{
		return _str < rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than or equal to rhs.
	bool operator<=(const string& rhs) const
	{
		return _str <= rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater thsn rhs.
	bool operator>(const string& rhs) const
	{
		return _str > rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater than or equal to rhs.
	bool operator>=(const string& rhs) const
	{
		return _str >= rhs._str;
	}

	// Append back.
	// string("this") + "rhs" == string("thisrhs")
	// @param rhs: append rhs back this string.
	// @return: ref this string.
	string& operator+=(const string& rhs)
	{
		_str += rhs._str;
		_surrogate_pair_count += rhs._surrogate_pair_count;
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
	/*template<typename T>
	string& operator+=(T rhs)
	{
		operator+=(to_string(rhs));
		return *this;
	}*/

	// Append back, get a new string instance without modify this string.
	// @param rhs: append rhs back this string.
	// @return: a new result string instance.
	/*template<typename T>
	string operator+(T rhs)
	{
		string str = *this;
		str += rhs;
		return str;
	}*/

	// Get a new substring from specific position with specific size
	// string("abcdefg").substring(2, 3) == string("cde");
	// @param from: from where to start, 0 if from begin.
	// @param size: how many chars you want.
	// @return: the new substring instance
	string substring(size_t from, size_t size = SIZE_MAX) const
	{
		const size_t uint16_size = std::min(size, length() - from);
		if (_surrogate_pair_count == 0)
		{
			/*ret._str.reserve(uint16_size + 1);
			ret._str.insert(ret._str.cbegin(), _str.cbegin() + from, _str.cbegin() + (from + uint16_size));*/
			return _str.substr(from, size);
		}
		else
		{
			auto from_it = helper::string::codepoint_count(_str.cbegin(), from, _str.cend());

			const size_t substr_surrogate_pair_count = _surrogate_pair_count - ((from_it - _str.cbegin()) - from);
			const size_t real_size = uint16_size + substr_surrogate_pair_count;

			string ret = _str.substr(from_it - _str.cbegin(), real_size);
			ret._surrogate_pair_count = substr_surrogate_pair_count;
			return ret;
		}
	}

	// Get the index of specific string
	// string("abcdefg").index_of("cde") == 2;
	// @param substr: substring to search.
	// @param from: from where to search.
	// @param length: how many length in this string to search, that means, searching from "from" to "from + length".
	// @return: the new substring instance
	size_t index_of(const string& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		// f(a) = c
		// f(a+b) = c+d
		// d = f(a+b)-c
		const size_t index = position_codepoint_to_index(from);	// index <-> c
		const size_t b = std::min(length, _str.size() - from);
		const size_t real_size = position_codepoint_to_index(from + b) - index;

		auto& predicate = helper::character::case_predicate<wchar_t>(cs);

		auto it = std::search(
			_str.cbegin() + index, _str.cbegin() + index + real_size
			, substr._str.cbegin(), substr._str.cend(),
			predicate
		);

		if (it == _str.cend()) return SIZE_MAX;

		const size_t index_found = it - _str.cbegin();
		return position_index_to_codepoint(index_found);

	}

	string& replace(size_t from, size_t count, const string& dest, case_sensitivity cs = case_sensitivity::sensitive)
	{
		size_t len = _str.size();

		count = position_codepoint_to_index(from + count);
		from = position_codepoint_to_index(from);
		count -= from;

		_str.replace(_str.cbegin() + from, _str.cbegin() + from + count, dest._str.c_str());
		calculate_surrogate();

		return *this;
	}

	string& replace(const string& src, const string& dest, case_sensitivity cs = case_sensitivity::sensitive)
	{
		// src should NOT be empty!
		if (src.is_empty()) return *this; // ASSERT!

		size_t len = _str.size();
		size_t index = 0;
		index = index_of(src, index, SIZE_MAX, cs);

		while (index < len)
		{
			_str.replace(_str.cbegin() + index, _str.cbegin() + index + src._str.size(), dest._str.c_str());
			// split into two steps beware of negative number when use type size_t
			_surrogate_pair_count += dest._surrogate_pair_count;
			_surrogate_pair_count -= src._surrogate_pair_count;
			index += src._str.size() - 1;
			index = index_of(src, index, SIZE_MAX, cs);
		}
		return *this;
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

	/*template<typename...Args>
	string format(Args&&...args) const
	{
		return fmt::format(_str.c_str(), intern::go_sv(std::forward<Args>(args)...));
	}*/

private:

	void calculate_surrogate()
	{
		_surrogate_pair_count = helper::string::count_surrogate_pair(_str.cbegin(), _str.cend());
	}

	size_t position_codepoint_to_index(size_t codepoint_count) const
	{
		auto from_it = helper::string::codepoint_count(_str.cbegin(), codepoint_count, _str.cend());
		return from_it - _str.cbegin();
	}

	size_t position_index_to_codepoint(size_t index) const
	{
		return index - helper::string::count_surrogate_pair(_str.cbegin(), _str.cbegin() + index);
	}


private:

	std::wstring _str;

	size_t _surrogate_pair_count = 0;

};

_NS_OSTR_END

template<>
struct fmt::formatter<ostr::string, wchar_t>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(ostr::string str, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), L"{}", str._str);
	}
};
