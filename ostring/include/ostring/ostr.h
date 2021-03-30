#pragma once
#include "definitions.h"
#include <string>
#include <string_view>
#include <algorithm>
#include "helpers.h"
#include "osv.h"

_NS_OSTR_BEGIN

class OPEN_STRING_EXPORT string
{
	friend class string_view;
	friend struct fmt::formatter<ostr::string, wchar_t>;
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
	string(const char* src, size_t length = SIZE_MAX);

	// Initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of wide characters,
	// and the endian it is.
	// @param src: the c-style wide char.
	// @param ec: what's the endian it is.
	string(const char16_t* src, size_t length = SIZE_MAX)
		:_surrogate_pair_count(0)
		, _str(src, 0, length)
	{
		calculate_surrogate();
	}

	// Initializes a new instance of the string class with multi count of specific char.
	// cccc..[count]..cccc
	// @param c: the char used to init.
	// @param count: how may c.
	string(const char16_t c, size_t count = 1)
		: _surrogate_pair_count(0)
	{
		// ansi as well as BMP in first plane can trans to wide char without side effect
		_str.resize(count, c);
	}

	// Initializes a new instance of the string class with std::wstring.
	// @param str: the wstring used to init.
	// @param count: how may c.
	string(const std::u16string& str)
		: _str(str)
	{
		calculate_surrogate();
	}

	// Initializes a new instance of the string class with std::string.
	// @param str: the string used to init.
	// @param count: how may c.
	string(const std::string& str)
		: string(str.c_str())
	{}

	string(const string_view& sv)
		: string(sv.raw().data(), sv.origin_length())
	{}

	string(const std::u16string_view& wr)
		: string(wr.data(), wr.size())
	{}

	operator const string_view() const
	{
		return to_sv();
	}

	string_view to_sv() const
	{
		return _str.c_str();
	}

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

	int compare(const string& rhs) const
	{
		return _str.compare(rhs._str);
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
		_str.append(to_string(rhs));
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
	string substring(size_t from, size_t size = SIZE_MAX) const;

	// Get the index of specific string
	// string("abcdefg").index_of("cde") == 2;
	// @param substr: substring to search.
	// @param from: from where to search.
	// @param length: how many length in this string to search, that means, searching from "from" to "from + length".
	// @return: the new substring instance

	size_t index_of(const string& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		return static_cast<string_view>(*this).substring(from, length).index_of(substr, cs) + from;
	}

	// Get the last index of specific string
	// string("123321123").last_index_of("123") == 6;
	// @param substr: substring to search.
	// @param from: from where to search.
	// @param length: how many length in this string to search, that means, searching from "from" to "from + length".
	// @return: the new substring instance
	/*size_t last_index_of(const string& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const
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

	}*/

	bool split(const string_view& splitter, string_view* lhs, string_view* rhs) const
	{
		string_view sv = *this;
		return sv.split(splitter, lhs, rhs);
	}

	size_t split(const string_view& splitter, std::vector<string_view>& str) const
	{
		string_view sv = *this;
		return sv.split(splitter, str);
	}

	template<typename F>
	size_t search(F&& predicate)
	{
		return std::find_if(_str.cbegin(), _str.cend(), std::forward<F>(predicate)) - _str.cbegin();
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
			index += dest._str.size();
			index = index_of(src, index, SIZE_MAX, cs);
		}
		return *this;
	}

	// Returns a new string in which all occurrences of a specified string in the current instance
	// are replaced with another specified string.
	// @return: how many substrings have been replaced
	string replace_copy(const string& src, const string& dest, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		string new_inst(*this);
		new_inst.replace(src, dest, cs);
		return new_inst;
	}

	template<typename...Args>
	string format(Args&&...args) const
	{
		return fmt::format(_str.c_str(), go_str(std::forward<Args>(args))...);
	}

	void trim_start()
	{
		_str.erase(_str.begin(), std::find_if(_str.begin(), _str.end(), [](auto ch) {
			return !std::isspace(ch);
		}));
	}

	void trim_end()
	{
		_str.erase(std::find_if(_str.rbegin(), _str.rend(), [](auto ch) {
			return !std::isspace(ch);
		}).base(), _str.end());
	}

	void trim()
	{
		trim_start();
		trim_end();
	}

	string trim_start_copy()
	{
		string ret(*this);
		ret.trim_start();
		return ret;
	}

	string trim_end_copy()
	{
		string ret(*this);
		ret.trim_end();
		return ret;
	}

	string trim_copy()
	{
		string ret(*this);
		ret.trim();
		return ret;
	}

	std::u16string_view raw() const
	{
		return _str;
	}

private:

	void calculate_surrogate()
	{
		_surrogate_pair_count = helper::string::count_surrogate_pair(_str.cbegin(), _str.cend());
	}

	size_t position_codepoint_to_index(size_t codepoint_count_to_iterator) const
	{
		auto from_it = helper::string::codepoint_count_to_iterator(_str.cbegin(), codepoint_count_to_iterator, _str.cend());
		return from_it - _str.cbegin();
	}

	size_t position_index_to_codepoint(size_t index) const
	{
		return index - helper::string::count_surrogate_pair(_str.cbegin(), _str.cbegin() + index);
	}

	template<class T>
	struct is_c_str : std::integral_constant
		<
		bool,
		std::is_same<char const*, typename std::decay<T>::type>::value ||
		std::is_same<char*, typename std::decay<T>::type>::value
		>
	{};

	template<typename T, typename = std::enable_if_t<!is_c_str<T>::value>>
	static auto&& go_str(T&& t)
	{
		return std::forward<T>(t);
	}

	template<typename T, typename = void, typename = std::enable_if_t<is_c_str<T>::value>>
	static auto go_str(T&& t)
	{
		return string(t);
	}

private:

	std::u16string _str;

	size_t _surrogate_pair_count = 0;

};

_NS_OSTR_END

template<>
struct fmt::formatter<ostr::string, char16_t>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(ostr::string str, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), u"{}", str.raw());
	}
};
