#pragma once
#include "definitions.h"
#include "char_types.h"
#include <vector>
#include "char_types.h"
#include "helpers.h"

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

	// initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of characters,
	// and encoding
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

	string(const wide_char* src, endian e = endian::big)
		:_surrogate_pair_count(0)
	{
		// assume big endian temporarily
		size_t len = wcslen(src) + 1;
		_wa.reserve(len);
		_wa.insert(_wa.cend(), src, src + len); 
		calculate_surrogate();
	}

	// ansi as well as BMP can trans to wide char without side effect
	string(const wide_char c, size_t count = 1)
		:_surrogate_pair_count(0)
	{
		_wa.resize(count, c); 
		append_terminal();
	}

	string(const string& str, size_t count)
	{
		_wa.reserve(str.length() * count + 1);
		for (size_t i = 0; i < count; ++i)
			operator+=(str);
	}

	// get how may characters in this string
	size_t length() const
	{
		const size_t length_with_0 = _wa.size() - _surrogate_pair_count;
		return length_with_0 > 0 ? length_with_0 - 1 : length_with_0;
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

	bool operator==(const string& rhs) const
	{
		return string_compare(rhs) == 0;
	}

	bool operator!=(const string& rhs) const
	{
		return string_compare(rhs) != 0;
	}

	bool operator<(const string& rhs) const
	{
		return string_compare(rhs) < 0;
	}

	bool operator<=(const string& rhs) const
	{
		return string_compare(rhs) <= 0;
	}

	bool operator>(const string& rhs) const
	{
		return string_compare(rhs) > 0;
	}

	bool operator>=(const string& rhs) const
	{
		return string_compare(rhs) >= 0;
	}

	// append back
	string& operator+=(const string& rhs)
	{
		auto it_target = _wa.cend() - (_wa.size() ? 1 : 0);
		_wa.insert(it_target, rhs._wa.cbegin(), rhs._wa.cend() - 1);
		_surrogate_pair_count += rhs._surrogate_pair_count; 
		append_terminal();
		return *this;
	}

	string operator+(const string& rhs)
	{
		string str = *this;
		str += rhs;
		return str;
	}

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

	size_t index_of(const string& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		const size_t index = position_codepoint_to_index(from);
		const size_t real_size = std::min(length, _wa.size() - index);
		return position_index_to_codepoint(helper::string::string_search(_wa.data() + index, real_size, substr._wa.data(), substr.length(), cs) + index);
	}

	// replace the substring which match param src into dest
	// @return: how many substrings have been replaced
	size_t replace(const string& src, const string& dest)
	{
		// @TODO
		return SIZE_MAX;
	}

	string replace(const string& src, const string& dest) const
	{
		// @TODO
		return string();
	}

	string format(...) const
	{
		// @TODO
		return string();
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


private:

	std::vector<char16_t> _wa;	// wchar_t array, NOT ends with '\0'

	size_t _surrogate_pair_count = 0;

};

_NS_OSTR_END
