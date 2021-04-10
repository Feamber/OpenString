#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "definitions.h"
#include "helpers.h"

#include <assert.h>

_NS_OSTR_BEGIN

namespace ofmt {

	template<typename T>
	bool to_string(const T& arg, std::u16string_view param, std::u16string& out) = delete;

	template <class T, class Traits>
	inline bool to_string(const std::basic_string_view<T, Traits>& arg, std::u16string_view param, std::u16string& out)
	{
		out.append(arg.cbegin(), arg.cend());
		return true;
	}

	template <class T, class Traits>
	inline bool to_string(const std::basic_string<T, Traits>& arg, std::u16string_view param, std::u16string& out)
	{
		out.append(arg.cbegin(), arg.cend());
		return true;
	}

	template <class T, size_t N>
	using cstr_arr = const T(&)[N];

	template <class T, size_t N>
	inline bool to_string(cstr_arr<T, N>& arg, std::u16string_view param, std::u16string& out)
	{
		std::basic_string_view<T> sv(arg);
		out.append(sv.cbegin(), sv.cend());
		return true;
	}

	template <class T>
	using cstr_ptr = const T*&;

	template <typename T>
	inline bool to_string(cstr_ptr<T>& arg, std::u16string_view param, std::u16string& out)
	{
		std::basic_string_view<T> sv(arg);
		out.append(sv.cbegin(), sv.cend());
		return true;
	}

	template<>
	inline bool to_string<std::u16string_view>(const std::u16string_view& arg, std::u16string_view param, std::u16string& out)
	{
		out = arg;
		return true;
	}

	template<>
	inline bool to_string<int>(const int& arg, std::u16string_view param, std::u16string& out)
	{
		helper::string::from_int(arg, out);
		return true;
	}

	template<>
	inline bool to_string<float>(const float& arg, std::u16string_view param, std::u16string& out)
	{
		helper::string::from_float_round(arg, out);
		return true;
	}


	template<typename Arg0>
	inline void to_string_index(size_t index, int alignment, std::u16string& out, std::u16string_view param, Arg0&& a0)
	{
		if (index != 0) return;

		// TODO: performance hit here when construct string
		//	|	|	|	|	|	|	|	|
		//	v	v	v	v	v	v	v	v
		std::u16string str;
		//	^	^	^	^	^	^	^	^
		//	|	|	|	|	|	|	|	|
		// TODO: performance hit here when construct string

		to_string(a0, param, str);

		size_t alignment_abs = std::abs(alignment);

		if (alignment_abs > str.size())
		{
			size_t space_count = alignment_abs - str.size();
			if (alignment < 0)
			{
				str.append(space_count, u' ');
			}
			else
			{
				str = std::u16string(space_count, u' ').append(str);
			}
		}
		out.append(str);
	}

	template<typename Arg0, typename...Args>
	inline void to_string_index(size_t index, int alignment, std::u16string& out, std::u16string_view param, Arg0&& a0, Args&&...args)
	{
		if (index == 0)
		{
			to_string_index(0, alignment, out, param, std::forward<Arg0>(a0));
			return;
		}
		to_string_index(index - 1, alignment, out, param, std::forward<Args>(args)...);
	}

	template<typename...Args>
	inline std::u16string format(std::u16string_view fmt)
	{
		return std::u16string(fmt);
	}

	template<typename...Args>
	inline std::u16string format(std::u16string_view fmt, Args&&...args)
	{
		size_t prev_holder = 0;

		// TODO: performance hit here when construct string
		//	|	|	|	|	|	|	|	|
		//	v	v	v	v	v	v	v	v
		std::u16string ans;
		//	^	^	^	^	^	^	^	^
		//	|	|	|	|	|	|	|	|
		// TODO: performance hit here when construct string

		int auto_index = -1;
		
		for (size_t i = 0; i < fmt.size(); ++i)
		{
			char16_t c = fmt.data()[i];
			if (c == u'{')
			{
				++i;
				// "{{" -> "{"
				if (fmt.data()[i] == u'{')
				{
					ans.append(fmt.substr(prev_holder, i - prev_holder));
					prev_holder = i + 1;
					continue;
				}
				if (fmt.data()[i] == u'}')
				{
					++auto_index;
				}
				ans.append(fmt.substr(prev_holder, i - prev_holder - 1));
				size_t index = 0;
				size_t param_colon = SIZE_MAX;
				int alignment = 0;
				
				for (; i < fmt.size(); ++i)
				{
					c = fmt.data()[i];
					if (ostr::helper::character::is_number(c))
					{
						assert(auto_index == -1 && "auto index not allowed when using manual index.");
						index = index * 10 + helper::character::to_number(c);
						continue;
					}
					if (c == u',')
					{
						++i;
						int sign = 1;
						if (fmt.data()[i] == u'-')
						{
							sign = -1;
							++i;
						}
						else if (fmt.data()[i] == u'+')
						{
							++i;
						}
						for (; i < fmt.size(); ++i)
						{
							c = fmt.data()[i];
							if (helper::character::is_number(c))
							{
								alignment = alignment * 10 + helper::character::to_number(c);
								continue;
							}
							if (c == u':' || c == u'}')
							{
								alignment *= sign;
								break;
							}
						}
					}
					if (c == u':')
					{
						param_colon = i;
						for (; i < fmt.size(); ++i)
						{
							if (fmt.data()[i] == u'}')
							{
								c = fmt.data()[i];
								break;
							}
						}
					}
					if (c == u'}')
					{
						to_string_index(
							(auto_index == -1) ? index : auto_index,
							alignment,
							ans,
							(param_colon == SIZE_MAX) ? u"" : fmt.substr(param_colon, i - param_colon),
							std::forward<Args>(args)...
						);
						prev_holder = i + 1;
						break;
					}
					else
					{
						assert(false && "unknown format.");
					}
				}
					
			}
			// "}}" -> "}"
			else if (c == u'}')
			{
				++i;
				if (fmt.data()[i] == u'}')
				{
					ans.append(fmt.substr(prev_holder, i - prev_holder));
					prev_holder = i + 1;
					continue;
				}
			}
		}
		ans.append(fmt.substr(prev_holder));

		return ans;
	}


}

_NS_OSTR_END
