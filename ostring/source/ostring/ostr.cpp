
#include <string_view>
#include "ostring/ostr.h"

_NS_OSTR_BEGIN

string::string(const char* src, size_t length)
	:_surrogate_pair_count(0)
{
	std::string_view sv(src, length);

	// ansi is a subset of the first plane
	//_str.insert(_str.cbegin(), src, src + len);
	_str = std::u16string(sv.cbegin(), sv.cend());
}

size_t string::length() const
{
	return _str.size() - _surrogate_pair_count;
}

string& string::operator+=(const string& rhs)
{
	_str += rhs._str;
	_surrogate_pair_count += rhs._surrogate_pair_count;
	return *this;
}

string string::operator+(const string& rhs)
{
	string str = *this;
	str += rhs;
	return str;
}

string string::substring(size_t from, size_t size) const
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
		auto from_it = helper::string::codepoint_count_to_iterator(_str.cbegin(), from, _str.cend());

		const size_t substr_surrogate_pair_count = _surrogate_pair_count - ((from_it - _str.cbegin()) - from);
		const size_t real_size = uint16_size + substr_surrogate_pair_count;

		string ret = _str.substr(from_it - _str.cbegin(), real_size);
		ret._surrogate_pair_count = substr_surrogate_pair_count;
		return ret;
	}
}

size_t string::index_of(const string& substr, size_t from, size_t length, case_sensitivity cs) const
{
	return static_cast<string_view>(*this).substring(from, length).index_of(substr, cs) + from;
}

bool string::split(const string_view& splitter, string_view* lhs, string_view* rhs) const
{
	string_view sv = *this;
	return sv.split(splitter, lhs, rhs);
}

size_t string::split(const string_view& splitter, std::vector<string_view>& str) const
{
	string_view sv = *this;
	return sv.split(splitter, str);
}

string& string::replace(size_t from, size_t count, const string& dest, case_sensitivity cs)
{
	size_t len = _str.size();

	count = position_codepoint_to_index(from + count);
	from = position_codepoint_to_index(from);
	count -= from;

	_str.replace(_str.cbegin() + from, _str.cbegin() + from + count, dest._str.c_str());
	calculate_surrogate();

	return *this;
}

string& string::replace(const string& src, const string& dest, case_sensitivity cs)
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
		index += dest._str.size();
		index = index_of(src, index, SIZE_MAX, cs);
	}
	return *this;
}

string string::replace_copy(const string& src, const string& dest, case_sensitivity cs) const
{
	string new_inst(*this);
	new_inst.replace(src, dest, cs);
	return new_inst;
}

void string::trim_start()
{
	_str.erase(_str.begin(), std::find_if(_str.begin(), _str.end(), [](auto ch) {
		return !std::isspace(ch);
	}));
}

void string::trim_end()
{
	_str.erase(std::find_if(_str.rbegin(), _str.rend(), [](auto ch) {
		return !std::isspace(ch);
	}).base(), _str.end());
}

string string::trim_start_copy()
{
	string ret(*this);
	ret.trim_start();
	return ret;
}

string string::trim_end_copy()
{
	string ret(*this);
	ret.trim_end();
	return ret;
}

string string::trim_copy()
{
	string ret(*this);
	ret.trim();
	return ret;
}

void string::calculate_surrogate()
{
	_surrogate_pair_count = helper::string::count_surrogate_pair(_str.cbegin(), _str.cend());
}

size_t string::position_codepoint_to_index(size_t codepoint_count_to_iterator) const
{
	auto from_it = helper::string::codepoint_count_to_iterator(_str.cbegin(), codepoint_count_to_iterator, _str.cend());
	return from_it - _str.cbegin();
}

size_t string::position_index_to_codepoint(size_t index) const
{
	return index - helper::string::count_surrogate_pair(_str.cbegin(), _str.cbegin() + index);
}

_NS_OSTR_END
