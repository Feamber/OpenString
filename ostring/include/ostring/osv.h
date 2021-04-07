#pragma once

#include <string_view>
#include <algorithm>
#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t
#include "fmt/format.h"
#include "definitions.h"
#include "types.h"
#include "helpers.h"
#include "coder.h"

_NS_OSTR_BEGIN

class OPEN_STRING_EXPORT string_view
{
public:

	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = codepoint;
		using pointer = value_type*;
		using reference = value_type&;

		using pointer_const = const codepoint*;  // or also value_type*

		using raw_it = std::u16string_view::iterator;
		using raw_const_it = std::u16string_view::const_iterator;

		// Should pass param end because method refresh_cp only work for a valid iterator;
		iterator(raw_it it, raw_const_it begin, raw_const_it end)
			: _it(it) 
			, _begin(begin)
			, _end(end)
		{
			refresh_cp();
		}

		// const reference operator*() const { return _cp; }	// compile error!
		const codepoint& operator*() const { return _cp; }
		pointer_const operator->() { return &_cp; }

		// Prefix increment
		iterator& operator++() 
		{ 
			_it += _len; 
			refresh_cp();
			return *this; 
		}

		// Postfix increment
		iterator operator++(int) 
		{ 
			iterator tmp = *this; 
			++(*this); 
			return tmp; 
		}

		size_t get_origin_index() const
		{
			return _it - _begin;
		}

		small_size_t get_origin_length() const
		{
			return _len;
		}

		bool operator== (const iterator& rhs) { return _it == rhs._it; }
		bool operator!= (const iterator& rhs) { return _it != rhs._it; }
		bool operator< (const iterator& rhs) { return _it < rhs._it; }
		bool operator> (const iterator& rhs) { return _it > rhs._it; }
		bool operator<= (const iterator& rhs) { return _it <= rhs._it; }
		bool operator>= (const iterator& rhs) { return _it >= rhs._it; }
		size_t operator- (const iterator& rhs) { return (_it - rhs._it) - helper::string::count_surrogate_pair(rhs._it, _it); }

	private:

		void refresh_cp()
		{
			if (_it == _end) return;
			helper::codepoint::utf16_to_utf32(_it.operator->(), _len, _cp);
		}

	private:

		raw_it _it;
		raw_const_it _begin;
		raw_const_it _end;
		codepoint _cp;
		small_size_t _len;

	};

	using const_iterator = iterator;

	iterator begin() { return iterator(_str.begin(), _str.cbegin(), _str.cend()); }
	iterator end() { return iterator(_str.end(), _str.cbegin(), _str.cend()); }
	const_iterator cbegin() const { return iterator(_str.cbegin(), _str.cbegin(), _str.cend()); }
	const_iterator cend() const { return iterator(_str.cend(), _str.cbegin(), _str.cend()); }
	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }

	constexpr string_view() noexcept = default;

	constexpr string_view(const string_view&) noexcept = default;

	constexpr string_view(const char16_t* str) noexcept
		: _str(str)
	{}

	constexpr string_view(const char16_t* str, size_t count) noexcept
		: _str(str, count)
	{}

	constexpr string_view(std::u16string_view sv) noexcept
		: _str(sv)
	{}
	
	[[nodiscard]] int compare(const string_view& rhs) const noexcept;

	[[nodiscard]] bool operator==(const string_view& rhs) const noexcept;

	[[nodiscard]] inline bool operator!=(const string_view& rhs) const noexcept
	{
		return !operator==(rhs);
	}

	[[nodiscard]] inline bool operator<(const string_view& rhs) const noexcept
	{
		return compare(rhs) < 0;
	}

	[[nodiscard]] inline bool operator<=(const string_view& rhs) const noexcept
	{
		return compare(rhs) <= 0;
	}

	[[nodiscard]] inline bool operator>(const string_view& rhs) const noexcept
	{
		return compare(rhs) > 0;
	}

	[[nodiscard]] inline bool operator>=(const string_view& rhs) const noexcept
	{
		return compare(rhs) >= 0;
	}

	[[nodiscard]] inline constexpr size_t origin_length() const noexcept
	{
		return _str.length();
	}

	[[nodiscard]] size_t length() const noexcept;

	[[nodiscard]] constexpr bool is_empty() const noexcept
	{
		return origin_length() == 0;
	}

	[[nodiscard]] string_view remove_prefix(size_t count) const noexcept;

	[[nodiscard]] string_view remove_suffix(size_t count) const noexcept;

	[[nodiscard]] string_view left(size_t count) const noexcept;

	[[nodiscard]] string_view right(size_t count) const noexcept;
	
	[[nodiscard]] string_view substring(size_t offset = 0, size_t count = SIZE_MAX) const noexcept;

	[[nodiscard]] size_t index_of(const string_view& pattern, case_sensitivity cs = case_sensitivity::sensitive) const noexcept;

	[[nodiscard]] size_t last_index_of(string_view pattern, case_sensitivity cs = case_sensitivity::sensitive) const noexcept;

	template<typename F>
	[[nodiscard]] size_t search(F&& predicate) const noexcept
	{
		auto it_begin = cbegin();
		auto it_end = cend();
		auto it = std::find_if(it_begin, it_end, std::forward<F>(predicate));
		if (it == it_end) return SIZE_MAX;
		return it - it_begin;
	}

	bool split(const string_view& splitter, string_view* lhs, string_view* rhs) const noexcept;

	size_t split(const string_view& splitter, std::vector<string_view>& str, bool cull_empty = false) const noexcept;

	[[nodiscard]] inline bool start_with(const string_view& sv_start) const noexcept
	{
		return substring(0, sv_start.length()) == sv_start;
	}

	[[nodiscard]] inline bool end_with(const string_view& sv_start) const noexcept
	{
		return substring(length() - sv_start.length(), sv_start.length()) == sv_start;
	}

	[[nodiscard]] string_view trim_start() const noexcept;

	[[nodiscard]] string_view trim_end() const noexcept;

	[[nodiscard]] inline string_view trim() const noexcept
	{
		return trim_start().trim_end();
	}

	[[nodiscard]] int to_int() const noexcept;

	// format string
	// format rule: fmtlib @ https://github.com/fmtlib/fmt
	template<typename...Args>
	[[nodiscard]] std::u16string format(Args&&...args) const
	{
		return fmt::format(_str, std::forward<Args>(args)...);
	}

	[[nodiscard]] constexpr std::u16string_view raw() const noexcept
	{
		return _str;
	}

	[[nodiscard]] bool encode_to_utf8(std::string& u8) const noexcept
	{
		return coder::convert_append(_str, u8);
	}

private:

	size_t position_codepoint_to_index(size_t codepoint_count_to_iterator) const noexcept;

	size_t position_index_to_codepoint(size_t index) const noexcept;

	void convert_codepoint_into_index(size_t& from, size_t& count) const noexcept;

private:

	std::u16string_view _str;
};

namespace literal
{
	[[nodiscard]] inline constexpr string_view operator""_o(const char16_t* str, size_t len) noexcept
	{
		return string_view(str, len);
	}
}

_NS_OSTR_END 

template<>
struct fmt::formatter<ostr::string_view, char16_t>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(ostr::string_view sv, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), u"{}", sv.raw());
	}
};

