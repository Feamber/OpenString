#pragma once

#include "definitions.h"
#include "char_types.h"
#include "helpers.h"
#include <string_view>
#include <algorithm>

#include "../fmt/include/fmt/format.h"
#pragma comment(lib, "fmt.lib")

_NS_OSTR_BEGIN

class string_view
{
public:

	constexpr string_view()
		: _str()
	{}
	constexpr string_view(const string_view&) 
		= default;

	constexpr string_view(const char16_t* str)
		: _str(str)
	{}
	constexpr string_view(const char16_t* str, size_t count)
		: _str(str, count)
	{}
	
	int compare(const string_view& rhs) const
	{
		return _str.compare(rhs._str);
	}

	bool operator==(const string_view& rhs) const
	{
		if (length() != rhs.length()) return false;
		return compare(rhs) == 0;
	}

	bool operator!=(const string_view& rhs) const
	{
		return !operator==(rhs);
	}

	bool operator<(const string_view& rhs) const
	{
		return compare(rhs) < 0;
	}

	bool operator<=(const string_view& rhs) const
	{
		return compare(rhs) <= 0;
	}

	bool operator>(const string_view& rhs) const
	{
		return compare(rhs) > 0;
	}

	bool operator>=(const string_view& rhs) const
	{
		return compare(rhs) >= 0;
	}

	[[nodiscard]] constexpr size_t origin_length() const noexcept
	{
		return _str.length();
	}

	[[nodiscard]] size_t length() const noexcept
	{
		size_t count_sp = helper::string::count_surrogate_pair(_str.cbegin(), _str.cend());
		return _str.length() - count_sp;
	}

	[[nodiscard]] constexpr bool empty() const noexcept
	{
		return origin_length() == 0;
	}

	void remove_prefix(size_t count) noexcept
	{
		count = helper::string::codepoint_count_to_iterator(_str.cbegin(), count, _str.cend()) - _str.cbegin();
		_str.remove_prefix(count);
	}

	void remove_suffix(size_t count) noexcept
	{
		count = helper::string::codepoint_count_to_iterator_backward(_str.crbegin(), count, _str.crend()) - _str.crbegin();
		_str.remove_suffix(count);
	}

	string_view remove_prefix_copy(size_t count) noexcept
	{
		string_view sv(*this);
		sv.remove_prefix(count);
		return sv;
	}

	string_view remove_suffix_copy(size_t count) noexcept
	{
		string_view sv(*this);
		sv.remove_suffix(count);
		return sv;
	}
	
	[[nodiscard]] string_view substring(size_t offset = 0, size_t count = SIZE_MAX) const
	{
		convert_codepoint_into_index(offset, count);
		return string_view(_str.substr(offset, count));
	}

	[[nodiscard]] size_t index_of(const string_view& pattern, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		auto& predicate = helper::character::case_predicate<wchar_t>(cs);

		auto it = std::search(
			_str.cbegin(), _str.cend()
			, pattern._str.cbegin(), pattern._str.cend(),
			predicate
		);

		if (it == _str.cend()) return SIZE_MAX;

		const size_t index_found = it - _str.cbegin();
		return position_index_to_codepoint(index_found);
	}

	[[nodiscard]] size_t last_index_of(string_view pattern, case_sensitivity cs = case_sensitivity::sensitive) const
	{
		auto& predicate = helper::character::case_predicate<wchar_t>(cs);

		auto it = std::search(
			_str.crbegin(), _str.crend()
			, pattern._str.crbegin(), pattern._str.crend(),
			predicate
		);

		if (it == _str.crend()) return SIZE_MAX;
		const size_t index_found = _str.crend() - it - pattern.length();
		return position_index_to_codepoint(index_found);
	}

	template<typename F>
	size_t search(F&& predicate)
	{
		return std::find_if(_str.cbegin(), _str.cend(), std::forward<F>(predicate)) - _str.cbegin();
	}

	bool split(const string_view& splitter, string_view* lhs, string_view* rhs) const
	{
		const size_t splitter_index = index_of(splitter);
		if (splitter_index == SIZE_MAX) return false;
		if (lhs) *lhs = substring(0, splitter_index);
		if (rhs) *rhs = substring(splitter_index + 1);
		return true;
	}

	size_t split(const string_view& splitter, std::vector<string_view>& str, bool cull_empty = false) const
	{
		string_view lhs;
		string_view rhs = *this;
		size_t split_times = 0;
		while (rhs.split(splitter, &lhs, &rhs))
		{
			if(!cull_empty || !lhs.empty())
				str.push_back(std::move(lhs));
			++split_times;
		}
		if (!cull_empty || !rhs.empty())
			str.push_back(rhs);
		return split_times;
	}

	bool start_with(const string_view& sv_start) const
	{
		return substring(0, sv_start.length()) == sv_start;
	}

	bool ends_with(const string_view& sv_start) const
	{
		return substring(length() - sv_start.length(), sv_start.length()) == sv_start;
	}

	string_view trim_start() const
	{
		size_t begin = 0;
		while (_str.data()[begin] == ' ')
			++begin;
		return substring(begin);
	}

	string_view trim_end() const
	{
		size_t end = index_of(u" ");
		return substring(0, end);
	}

	string_view trim() const
	{
		return trim_start().trim_end();
	}

	[[nodiscard]] int to_int() const
	{
		int value = 0;
		string_view sv = trim();
		for (const auto& c : sv._str)
			value = value * 10 + c - OCHAR('0');
		return value;
	}

	// format string
	// format rule: fmtlib @ https://github.com/fmtlib/fmt
	template<typename...Args>
	std::u16string format(Args&&...args) const
	{
		return fmt::format(_str, std::forward<Args>(args)...);
	}

private:
	
	constexpr string_view(std::u16string_view&& sv)
		: _str(sv)
	{}

	size_t position_codepoint_to_index(size_t codepoint_count_to_iterator) const
	{
		auto from_it = helper::string::codepoint_count_to_iterator(_str.cbegin(), codepoint_count_to_iterator, _str.cend());
		return from_it - _str.cbegin();
	}

	size_t position_index_to_codepoint(size_t index) const
	{
		return index - helper::string::count_surrogate_pair(_str.cbegin(), _str.cbegin() + index);
	}

	void convert_codepoint_into_index(size_t& from, size_t& count) const
	{
		const size_t index = position_codepoint_to_index(from);
		const size_t b = std::min(count, _str.size() - from);
		const size_t real_size = position_codepoint_to_index(from + b) - index;
		from = index; 
		count = real_size;
	}

private:

	friend class string;
	friend struct fmt::formatter<ostr::string_view, char16_t>;

	std::u16string_view _str;
};

#pragma warning(push)
#pragma warning(disable: 455)
constexpr string_view operator""o(const char16_t* str, size_t len)
{
	return string_view(str, len);
}
#pragma warning(pop)

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
		return fmt::format_to(ctx.out(), u"{}", sv._str);
	}
};

