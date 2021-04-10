
#include <string_view>
#include "ostring/ostr.h"

_NS_OSTR_BEGIN

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

size_t string::index_of(const string_view& substr, size_t from, size_t length, case_sensitivity cs) const
{
	size_t ind = static_cast<string_view>(*this)
		.substring(from, length)
		.index_of(substr, cs);
	if (ind == SIZE_MAX) return SIZE_MAX;
	return ind + from;
}

size_t string::last_index_of(const string_view& substr, size_t from, size_t length, case_sensitivity cs) const
{
	size_t ind = static_cast<string_view>(*this)
		.substring(from, length)
		.last_index_of(substr, cs);
	if (ind == SIZE_MAX) return SIZE_MAX;
	return ind + from;
}

bool string::split(const string_view& splitter, string_view* lhs, string_view* rhs) const
{
	return to_sv().split(splitter, lhs, rhs);
}

size_t string::split(const string_view& splitter, std::vector<string_view>& str) const
{
	return to_sv().split(splitter, str);
}

string& string::replace_origin(size_t from, size_t count, const string_view& dest, case_sensitivity cs)
{
	count = position_codepoint_to_index(from + count);
	from = position_codepoint_to_index(from);
	count -= from;

	_str.replace(_str.cbegin() + from, _str.cbegin() + from + count, dest.raw());
	calculate_surrogate();

	return *this;
}

string& string::replace_origin(const string_view& src, const string_view& dest, case_sensitivity cs)
{
	// src should NOT be empty!
	if (src.is_empty()) return *this; // ASSERT!

	size_t len = _str.size();
	size_t index = 0;
	index = index_of(src, index, SIZE_MAX, cs);

	while (index < len)
	{
		_str.replace(_str.cbegin() + index, _str.cbegin() + index + src.raw().size(), dest.raw());
		index += dest.origin_length();
		index = index_of(src, index, SIZE_MAX, cs);
	}
	calculate_surrogate();
	return *this;
}

string string::replace_copy(const string_view& src, const string_view& dest, case_sensitivity cs) const
{
	string new_inst(*this);
	new_inst.replace_origin(src, dest, cs);
	return new_inst;
}

string& string::trim_start()
{
	auto begin = _str.cbegin();

	while (begin != _str.cend() && *begin == ' ') 
		++begin;

	_str.erase(_str.cbegin(), begin);
	return *this;
}

string& string::trim_end() 
{
	auto rbegin = _str.crbegin();

	while (rbegin != _str.crend() && *rbegin == ' ') 
		++rbegin;

	_str.erase(rbegin.base(), _str.cend());
	return *this;
}

string string::trim_start_copy() const
{
	string ret(*this);
	ret.trim_start();
	return ret;
}

string string::trim_end_copy() const
{
	string ret(*this);
	ret.trim_end();
	return ret;
}

string string::trim_copy() const
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
