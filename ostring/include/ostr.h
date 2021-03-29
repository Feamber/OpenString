#pragma once
#include "definitions.h"
#include <string>
#include <string_view>
#include <algorithm>
#include "helpers.h"
#include "osv.h"

_NS_OSTR_BEGIN

enum class encoding : char8_t
{
	ansi,
	utf8
};


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
	string(const char* src, size_t length = SIZE_MAX, encoding ec = encoding::ansi);

	// Initializes a new instance of the string class to the value 
	// indicated by a specified pointer to an array of wide characters,
	// and the endian it is.
	// @param src: the c-style wide char.
	// @param ec: what's the endian it is.
	string(const char16_t* src, size_t length = SIZE_MAX);

	// Initializes a new instance of the string class with multi count of specific char.
	// cccc..[count]..cccc
	// @param c: the char used to init.
	// @param count: how may c.
	string(const char16_t c, size_t count = 1);

	// Initializes a new instance of the string class with std::wstring.
	// @param str: the wstring used to init.
	// @param count: how may c.
	string(const std::u16string& str);

	// Initializes a new instance of the string class with std::string.
	// @param str: the string used to init.
	// @param count: how may c.
	string(const std::string& str);

	string(const string_view& sv);

	string(const std::u16string_view& wr);

	operator const string_view() const;

	string_view to_sv() const;

	// @return: the length of string.
	size_t length() const;

	// @return: whether this string is empty.
	bool is_empty() const;

	int compare(const string& rhs) const;

	// Are they totally equal?
	// @param rhs: another string.
	// @return: true if totally equal.
	bool operator==(const string& rhs) const;

	// Are they different?
	// @param rhs: another string.
	// @return: true if different.
	bool operator!=(const string& rhs) const;

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than rhs.
	bool operator<(const string& rhs) const;

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than or equal to rhs.
	bool operator<=(const string& rhs) const;

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater thsn rhs.
	bool operator>(const string& rhs) const;

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater than or equal to rhs.
	bool operator>=(const string& rhs) const;

	// Append back.
	// string("this") + "rhs" == string("thisrhs")
	// @param rhs: append rhs back this string.
	// @return: ref this string.
	string& operator+=(const string& rhs);

	// Append back, get a new string instance without modify this string.
	// string("this") + "rhs" == string("thisrhs")
	// @param rhs: append rhs back this string.
	// @return: a new result string instance.
	string operator+(const string& rhs);

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

	size_t index_of(const string& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const;

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

	bool split(const string_view& splitter, string_view* lhs, string_view* rhs) const;

	size_t split(const string_view& splitter, std::vector<string_view>& str) const;

	template<typename F>
	size_t search(F&& predicate)
	{
		return std::find_if(_str.cbegin(), _str.cend(), std::forward<F>(predicate)) - _str.cbegin();
	}

	string& replace(size_t from, size_t count, const string& dest, case_sensitivity cs = case_sensitivity::sensitive);

	string& replace(const string& src, const string& dest, case_sensitivity cs = case_sensitivity::sensitive);

	// Returns a new string in which all occurrences of a specified string in the current instance
	// are replaced with another specified string.
	// @return: how many substrings have been replaced
	string replace_copy(const string& src, const string& dest, case_sensitivity cs = case_sensitivity::sensitive) const;

	template<typename...Args>
	string format(Args&&...args) const
	{
		return fmt::format(_str.c_str(), go_str(std::forward<Args>(args))...);
	}

	void trim_start();

	void trim_end();

	void trim();

	string trim_start_copy();

	string trim_end_copy();

	string trim_copy();

private:

	void calculate_surrogate();

	size_t position_codepoint_to_index(size_t codepoint_count_to_iterator) const;

	size_t position_index_to_codepoint(size_t index) const;

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
