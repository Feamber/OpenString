#pragma once

#include "definitions.h"
#include "char_types.h"
#include "helpers.h"
#include <string_view>
#include <algorithm>
#include "string.h"

#include "../fmt/include/fmt/format.h"
#pragma comment(lib, "fmt.lib")

_NS_OSTR_BEGIN

class string_view
{
public:

	constexpr string_view()
		: _str()
		, _bit()
	{}
	constexpr string_view(const string_view&) = default;
	constexpr string_view(const char* str)
		: _bit(bitsize::_8)
		, _str(str)
	{}
	constexpr string_view(const wchar_t* str)
		: _bit(bitsize::_16)
		, _str(str)
	{}
	constexpr string_view(const char* str, size_t count)
		: _bit(bitsize::_8)
		, _str(str, count)
	{}
	constexpr string_view(const wchar_t* str, size_t count)
		: _bit(bitsize::_16)
		, _str(str, count)
	{}
	constexpr string_view(const string& str)
		: _bit(bitsize::_16)
		, _str(str._str.c_str())
	{}

	string to_string() const
	{
		return string(_bit == string_view::bitsize::_8 ? std::move(string(_str._8.data(), length())) : std::move(string(_str._16.data(), length())));
	}
	
	int compare(const string_view& rhs) const
	{
		switch (_bit)
		{
		case bitsize::_8:
		{
			switch (rhs._bit)
			{
			case bitsize::_8:
				return _str._8.compare(rhs._str._8);
			case bitsize::_16:
			{
				size_t i = 0;
				const size_t lhs_size = _str._8.size();
				const size_t rhs_size = rhs._str._16.size();
				const size_t limit = std::min(lhs_size, rhs_size);

				while (i < limit)
				{
					if (_str._8[i] != rhs._str._16[i])
						return _str._8[i] < rhs._str._16[i] ? -1 : 1;
					++i;
				}

				if (lhs_size < rhs_size)
					return -1;
				if (lhs_size > rhs_size)
					return 1;
				return 0;
			}
			}
			break;
		}
		case bitsize::_16:
		{
			switch (rhs._bit)
			{
			case bitsize::_8:
			{
				size_t i = 0;
				const size_t lhs_size = _str._16.size();
				const size_t rhs_size = rhs._str._8.size();
				const size_t limit = std::min(lhs_size, rhs_size);
				
				while (i < limit)
				{
					if (_str._16[i] != rhs._str._8[i])
						return _str._16[i] < rhs._str._8[i] ? -1 : 1;
					++i;
				}

				if(lhs_size < rhs_size)
					return -1;
				if (lhs_size > rhs_size)
					return 1;
				return 0;
			}
			case bitsize::_16:
				return _str._16.compare(rhs._str._16);
			}
			break;
		}
		};
		return 0;
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

	[[nodiscard]] constexpr size_t length() const noexcept
	{
		switch (_bit)
		{
		case bitsize::_8:
			return _str._8.length();
		case bitsize::_16:
			return _str._16.length();
		};
		return SIZE_MAX;
	}

	[[nodiscard]] constexpr bool empty() const noexcept
	{
		return length() == 0;
	}

	constexpr void remove_prefix(const size_t count) noexcept
	{
		switch (_bit)
		{
		case bitsize::_8:
			return _str._8.remove_prefix(count);
		case bitsize::_16:
			return _str._16.remove_prefix(count);
		};
	}

	constexpr void remove_suffix(const size_t count) noexcept
	{
		switch (_bit)
		{
		case bitsize::_8:
			return _str._8.remove_suffix(count);
		case bitsize::_16:
			return _str._16.remove_suffix(count);
		};
	}

	constexpr string_view remove_prefix_copy(const size_t count) noexcept
	{
		string_view sv(*this);
		sv.remove_prefix(count);
		return sv;
	}

	constexpr string_view remove_suffix_copy(const size_t count) noexcept
	{
		string_view sv(*this);
		sv.remove_suffix(count);
		return sv;
	}
	
	[[nodiscard]] constexpr string_view substring(const size_t offset = 0, size_t count = SIZE_MAX) const
	{
		size_t realsize = std::min(count, length() - offset);
		switch (_bit)
		{
		case bitsize::_8:
			return string_view(_str._8.substr(offset), realsize);
		case bitsize::_16:
			return string_view(_str._16.substr(offset), realsize);
		};
		return "INVALID STRING_VIEW";
	}

	[[nodiscard]] constexpr size_t index_of(string_view pattern, const size_t offset = 0) const
	{
		switch (_bit)
		{
		case bitsize::_8:
		{
			switch (pattern._bit)
			{
			case bitsize::_8:
				return _str._8.find_first_of(pattern._str._8, offset);
			case bitsize::_16:
			{
				return 
					std::find_first_of
					(
					_str._8.cbegin() + offset, _str._8.cend()
					, pattern._str._16.cbegin(), pattern._str._16.cend()
					) 
					- _str._8.cbegin();
			}
			}
		}
		case bitsize::_16:
		{
			switch (pattern._bit)
			{
			case bitsize::_8:
				return
					std::find_first_of
					(
						_str._16.cbegin() + offset, _str._16.cend()
						, pattern._str._8.cbegin(), pattern._str._8.cend()
					)
					- _str._16.cbegin();
			case bitsize::_16:
				return _str._16.find_first_of(pattern._str._16, offset);
			}
		}
		};
		return SIZE_MAX;
	}

	constexpr bool split(const string_view& splitter, string_view* lhs, string_view* rhs) const
	{
		const size_t splitter_index = index_of(splitter);
		if (splitter_index == SIZE_MAX) return false;
		if (lhs) *lhs = substring(0, splitter_index);
		if (rhs) *rhs = substring(splitter_index + 1);
		return true;
	}

	constexpr string_view trim_start() const
	{
		size_t begin = 0;

		switch (_bit)
		{
		case bitsize::_8:
			while (_str._8.data()[begin] == ' ')
				++begin;
			break;
		case bitsize::_16:
			while (_str._16.data()[begin] == ' ')
				++begin;
			break;
		};
		return substring(begin);
	}

	constexpr string_view trim_end() const
	{
		size_t end = index_of(" ");
		return substring(0, end);
	}

	constexpr string_view trim() const
	{
		return trim_start().trim_end();
	}

	[[nodiscard]] int to_int() const
	{
		int value = 0;
		string_view sv = trim();
		switch (_bit)
		{
		case bitsize::_8:
		{
			for (const auto& c : sv._str._8)
				value = value * 10 + c - '0';
			break;
		}
		case bitsize::_16:
		{
			for (const auto& c : sv._str._16)
				value = value * 10 + c - L'0';
			break;
		}
		};

		return value;
	}

	// format string
	// format rule: fmtlib @ https://github.com/fmtlib/fmt
	template<typename...Args>
	string format(Args&&...args) const
	{
		return fmt::format(to_string()._str, go_sv(std::forward<Args>(args))...);
	}

private:
	
	string_view(std::string_view&& sv, size_t count)
		: _str(std::move(sv), count)
		, _bit(bitsize::_8)
	{}

	string_view(std::wstring_view&& sv, size_t count)
		: _str(std::move(sv), count)
		, _bit(bitsize::_16)
	{}

	template<class T>
	struct is_c_str : std::integral_constant
		<
		bool,
		std::is_same<char const*, typename std::decay<T>::type>::value ||
		std::is_same<char*, typename std::decay<T>::type>::value
		>
	{};

	template<typename T, typename = std::enable_if_t<!is_c_str<T>::value>>
	static auto&& go_sv(T&& t)
	{
		return std::forward<T>(t);
	}
	template<typename T, typename = void, typename = std::enable_if_t<is_c_str<T>::value>>
	static auto go_sv(T&& t)
	{
		return string_view(t);
	}


private:

	friend class string;
	friend struct fmt::formatter<ostr::string_view, wchar_t>;

	union string_view_union
	{
		std::string_view _8;
		std::wstring_view _16;

		constexpr string_view_union()
			:_16()
		{}

		constexpr string_view_union(const char* str)
			: _8(str) {}
		constexpr string_view_union(const wchar_t* str)
			: _16(str) {}

		constexpr string_view_union(const char* str, size_t count)
			: _8(str, count) {}
		constexpr string_view_union(const wchar_t* str, size_t count)
			: _16(str, count) {}

		constexpr string_view_union(std::string_view&& str, size_t count)
			: _8(str.data(), count) {}
		constexpr string_view_union(std::wstring_view&& str, size_t count)
			: _16(str.data(), count) {}
	} _str;

	enum class bitsize : uint8_t
	{
		_8,
		_16
	} _bit;
};

#pragma warning(push)
#pragma warning(disable: 455)
constexpr string_view operator""o(const char* str, size_t len)
{
	return string_view(str, len);
}

constexpr string_view operator""o(const wchar_t* str, size_t len)
{
	return string_view(str, len);
}
#pragma warning(pop)

_NS_OSTR_END 

template<>
struct fmt::formatter<ostr::string_view, wchar_t>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(ostr::string_view sv, FormatContext& ctx)
	{
		if (sv._bit == ostr::string_view::bitsize::_8)
			return fmt::format_to(ctx.out(), L"{}", std::wstring(sv._str._8.cbegin(), sv._str._8.cend()));
		return fmt::format_to(ctx.out(), L"{}", sv._str._16);
	}
};


