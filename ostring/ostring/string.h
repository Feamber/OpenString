#pragma once
#include "definitions.h"
#include "char_types.h"
#include <vector>
#include "char_types.h"
#include "helpers.h"

_NS_OSTR_BEGIN

enum class encoding : uint8_t
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

	// initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of characters,
	// and encoding
	string(const ansi_char* src, encoding ec = encoding::ansi)
		:_surrogate_pair_count(0)
	{
		size_t len = strlen(src);
		_wa.reserve(len);

		switch (ec)
		{
		case encoding::ansi:
		{
			// ansi is a subset of the first plane
			_wa.insert(_wa.cend(), src, src + len);
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
				helper::codepoint::utf8_to_utf16((const uint8_t*)src, utf8_length, pair, utf16_length);
				_wa.insert(_wa.end(), pair, pair + utf16_length);
				if (utf16_length == 2) ++_surrogate_pair_count;
				src += utf8_length;
			}
			break;
		}
		default:
			(*(int*)0), "not supported encoding type when sending in ansi.";
			break;
		}
	}

	string(const wide_char* src, endian e = endian::big)
		:_surrogate_pair_count(0)
	{
		size_t len = wcslen(src);
		_wa.reserve(len);
		for (const wide_char* pw = src; pw < src + len - 1; ++pw)
			if (helper::codepoint::is_surrogate_pair(pw[0], pw[1]))
			{
				++_surrogate_pair_count;
				++pw;
			}
		_wa.insert(_wa.cend(), src, src + len);
	}

	// ansi as well as BMP can trans to wide char without side effect
	string(const wide_char c, size_t count)
		:_surrogate_pair_count(0)
	{
		_wa.resize(count, c);
	}

	string(const string& str, size_t count)
	{
		_wa.reserve(str.length() * count);
		for (size_t i = 0; i < count; ++i)
			operator+=(str);
	}

	// get how may characters in this string
	size_t length() const
	{
		return _wa.size() - _surrogate_pair_count;
	}

	// whether this string is empty
	bool is_empty() const
	{
		return length() == 0;
	}

	// compare two strings
	// return 1 if this is greater than rhs
	// return 0 if this is equal to rhs
	// return -1 if this is less than rhs
	int compare(const string& rhs) const
	{
		const size_t lhs_length = length();
		const size_t rhs_length = rhs.length();
		const int size_compare = helper::string::compare(lhs_length, rhs_length);
		if (size_compare != 0) return size_compare;
		const int cmp = helper::string::compare(_wa.data(), rhs._wa.data(), lhs_length);
		return cmp;
	}

	bool operator==(const string& rhs) const
	{
		return compare(rhs) == 0;
	}

	bool operator!=(const string& rhs) const
	{
		return compare(rhs) != 0;
	}

	bool operator<(const string& rhs) const
	{
		return compare(rhs) < 0;
	}

	bool operator<=(const string& rhs) const
	{
		return compare(rhs) <= 0;
	}

	bool operator>(const string& rhs) const
	{
		return compare(rhs) > 0;
	}

	bool operator>=(const string& rhs) const
	{
		return compare(rhs) >= 0;
	}

	// append back
	string& operator+=(const string& rhs)
	{
		_wa.insert(_wa.end(), rhs._wa.cbegin(), rhs._wa.cend());
		_surrogate_pair_count += rhs._surrogate_pair_count;
		return *this;
	}

	string operator+(const string& rhs)
	{
		string str = *this;
		str += rhs;
		return str;
	}

private:

	std::vector<uint16_t> _wa;	// wchar_t array, NOT ends with '\0'

	size_t _surrogate_pair_count = 0;

};

_NS_OSTR_END
