#pragma once
#include "definitions.h"
#include <string>
#include <string_view>
#include <algorithm>
#include "helpers.h"
#include "osv.h"

// TODO: move impl to cpp

_NS_OSTR_BEGIN

class OPEN_STRING_EXPORT string
{
public:

	string() = default;
	string(string&&) = default;
	string(const string&) = default;
	string& operator=(string&&) = default;
	string& operator=(const string&) = default;

	template<typename T, size_t N>
	string(const T arr[N])
	{
		using ut = std::make_unsigned_t< T >;
		using up = std::add_pointer_t< std::add_const_t< ut > >;
		std::basic_string_view<ut> sv(reinterpret_cast< up >(arr), N);
		_str = std::u16string(sv.cbegin(), sv.cend());
		calculate_surrogate();
	}

	template<typename T>
	string(const T* src)
	{
		using ut = std::make_unsigned_t< T >;
		using up = std::add_pointer_t< std::add_const_t< ut > >;
		std::basic_string_view<ut> sv(reinterpret_cast<up>(src));
		_str = std::u16string(sv.cbegin(), sv.cend());
		calculate_surrogate();
	}

	template<typename T>
	string(const T* src, size_t len)
	{
		using ut = std::make_unsigned_t< T >;
		using up = std::add_pointer_t< std::add_const_t< ut > >;
		std::basic_string_view<ut> sv(reinterpret_cast<up>(src), len);
		_str = std::u16string(sv.cbegin(), sv.cend());
		calculate_surrogate();
	}

	// Initializes a new instance of the string class with the value 
	// indicated by a specified pointer to a pointer to 16-bit characters,
	// and the endian it is.
	// @param src: the c-style 16-bit characters.
	string(const char16_t* src)
		: _str(src)
	{
		calculate_surrogate();
	}

	// Initializes a new instance of the string class with the value 
	// indicated by a specified pointer to a pointer to 16-bit characters,
	// and the endian it is.
	// @param src: the c-style wide char.
	// @param length: how many count to use.
	string(const char16_t* src, size_t length)
		: _str(src, length)
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

	template<typename T>
	string(const std::basic_string<T>& str)
	{
		using ut = std::make_unsigned_t< T >;
		_str = std::u16string((const ut*)str.data(), (const ut*)(str.data() + str.size()));
		calculate_surrogate();
	}

	template<typename T>
	string(std::basic_string_view<T> str)
	{
		using ut = std::make_unsigned_t< T >;
		_str = std::u16string((const ut*)str.data(), (const ut*)(str.data() + str.size()));
		calculate_surrogate();
	}

	string(const std::u16string& str)
		: _str(str)
	{
		calculate_surrogate();
	}

	string(std::u16string_view sv)
		: _str(sv)
	{
		calculate_surrogate();
	}

	string(string_view sv)
		: _str(sv.raw())
	{
		calculate_surrogate();
	}

	inline operator const string_view() const
	{
		return to_sv();
	}

	[[nodiscard]] inline string_view to_sv() const
	{
		return _str.c_str();
	}

	// @return: the length of string.
	[[nodiscard]] size_t length() const;

	// @return: whether this string is empty.
	[[nodiscard]] inline bool is_empty() const
	{
		return length() == 0;
	}

	[[nodiscard]] inline int compare(const string& rhs) const
	{
		return _str.compare(rhs._str);
	}

	// Are they totally equal?
	// @param rhs: another string.
	// @return: true if totally equal.
	[[nodiscard]] inline bool operator==(const string& rhs) const
	{
		return _str == rhs._str;
	}

	// Are they different?
	// @param rhs: another string.
	// @return: true if different.
	[[nodiscard]] inline bool operator!=(const string& rhs) const
	{
		return _str != rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than rhs.
	[[nodiscard]] inline bool operator<(const string& rhs) const
	{
		return _str < rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if less than or equal to rhs.
	[[nodiscard]] inline bool operator<=(const string& rhs) const
	{
		return _str <= rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater thsn rhs.
	[[nodiscard]] inline bool operator>(const string& rhs) const
	{
		return _str > rhs._str;
	}

	// Compare with unicode value.
	// @param rhs: another string.
	// @return: true if greater than or equal to rhs.
	[[nodiscard]] inline bool operator>=(const string& rhs) const
	{
		return _str >= rhs._str;
	}

	// Append back.
	// string("this") + "rhs" == string("thisrhs")
	// @param rhs: append rhs back this string.
	// @return: ref this string.
	string& operator+=(const string& rhs);

	// Append back, get a new string instance without modify this string.
	// string("this") + "rhs" == string("thisrhs")
	// @param rhs: append rhs back this string.
	// @return: a new result string instance.
	[[nodiscard]] string operator+(const string& rhs);

	// Get a new substring from specific position with specific size
	// string("abcdefg").substring(2, 3) == string("cde");
	// @param from: from where to start, 0 if from begin.
	// @param size: how many chars you want.
	// @return: the new substring instance
	[[nodiscard]] string substring(size_t from, size_t size = SIZE_MAX) const;

	// Get the index of specific string
	// string("abcdefg").index_of("cde") == 2;
	// @param substr: substring to search.
	// @param from: from where to search.
	// @param length: how many length in this string to search, that means, searching from "from" to "from + length".
	// @return: the index of where the string first found

	[[nodiscard]] size_t index_of(const string_view& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const;

	[[nodiscard]] size_t last_index_of(const string_view& substr, size_t from = 0, size_t length = SIZE_MAX, case_sensitivity cs = case_sensitivity::sensitive) const;

	bool split(const string_view& splitter, string_view* lhs, string_view* rhs) const;

	size_t split(const string_view& splitter, std::vector<string_view>& str) const;

	template<typename F>
	[[nodiscard]] size_t search(F&& predicate) const
	{
		return string_view(*this).search(predicate);
	}

	string& replace_origin(size_t from, size_t count, const string_view& dest, case_sensitivity cs = case_sensitivity::sensitive);

	string& replace_origin(const string_view& src, const string_view& dest, case_sensitivity cs = case_sensitivity::sensitive);

	// Returns a new string in which all occurrences of a specified string in the current instance
	// are replaced with another specified string.
	// @return: how many substrings have been replaced
	[[nodiscard]] string replace_copy(const string_view& src, const string_view& dest, case_sensitivity cs = case_sensitivity::sensitive) const;

	template<typename...Args>
	[[nodiscard]] string format(Args&&...args) const
	{
		return fmt::format(_str.c_str(), go_str(std::forward<Args>(args))...);
	}

	string& trim_start();

	string& trim_end();

	inline string& trim()
	{
		return trim_start().trim_end();
	}

	[[nodiscard]] string trim_start_copy() const;

	[[nodiscard]] string trim_end_copy() const;

	[[nodiscard]] string trim_copy() const;

	[[nodiscard]] inline std::u16string_view raw() const
	{
		return std::u16string_view( _str );
	}

	bool decode_from_utf8(std::string_view u8) noexcept
	{
		return coder::convert_append(u8, _str);
	}

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
