#pragma once
#include <vector>
#include <string>
#include <string_view>

#include <ostring/osv.h>
#include "definitions.h"

_NS_OSTR_BEGIN

namespace ofmt {

	template<typename T>
	struct is_char_type : std::false_type {};
	template<>
	struct is_char_type<char> : std::true_type {};
#ifdef __cpp_lib_char8_t
	template<>
	struct _is_char_type<char8_t> : std::true_type {};
#endif // __cpp_lib_char8_t
	template<>
	struct is_char_type<char16_t> : std::true_type {};
	template<>
	struct is_char_type<char32_t> : std::true_type {};
	template<>
	struct is_char_type<wchar_t> : std::true_type {};
	template<typename T>
	struct sv_trait 
	{
		using type = T;
	};
	template <class T, class Traits, class Alloc>
	struct sv_trait<std::basic_string<T, Traits, Alloc>>
	{
		using type = T;
	};
	template <class T, class Traits>
	struct sv_trait<std::basic_string_view<T, Traits>>
	{
		using type = T;
	};

	template<typename T>
	using char_base_t =
		std::remove_cv_t<
		std::remove_all_extents_t<
		std::remove_reference_t<
		std::remove_pointer_t<
		typename sv_trait<
		T>::type>>>>;

	using a = char_base_t<const char(&)[5]>;

	template<typename T>
	bool to_string(T&& arg, std::u16string_view param, std::u16string& out) = delete;

	/*template<typename T>
	inline bool to_string(T&& arg, std::u16string_view param, std::u16string& out)
	{
		using base_type = char_base_t<T>;
		static_assert(is_char_type<base_type>::value);
		std::basic_string_view<base_type> sv(arg);
		out.append(sv.cbegin(), sv.cend());
		return true;
	}*/

	template <class T, class Traits>
	inline bool to_string(std::basic_string_view<T, Traits>&& arg, std::u16string_view param, std::u16string& out)
	{
		out.append(arg.cbegin(), arg.cend());
		return true;
	}

	template <class T, size_t N>
	using cstr_arr = T(&)[N];

	template <class T, size_t N>
	inline bool to_string(cstr_arr<T, N>&& arg, std::u16string_view param, std::u16string& out)
	{
		out.append(arg);
		return true;
	}

	template <class T>
	using cstr_ptr = const T*&;

	template <typename T>
	inline bool to_string(cstr_ptr<T>&& arg, std::u16string_view param, std::u16string& out)
	{
		std::basic_string_view<T> sv(arg);
		out.append(sv.cbegin(), sv.cend());
		return true;
	}

	template<>
	inline bool to_string<std::u16string_view>(std::u16string_view&& arg, std::u16string_view param, std::u16string& out)
	{
		out = arg;
		return true;
	}

	template<>
	inline bool to_string<int>(int&& arg, std::u16string_view param, std::u16string& out)
	{
		int i = abs(arg);
		size_t size = (size_t)(floor(log10(i)) + 1) + (arg < 0 ? 1 : 0);
		out.append(size, u'0');
		if (arg < 0) {
			--size;
			out[size] = u'-';
		}
		for(int p = 0; p < size; ++p )
		{
			out[p] = (i % 10 + u'0');
			i /= 10;
		}
		std::reverse(out.begin(), out.end());
		return true;
	}


	template<typename Arg0, typename...Args>
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

		to_string(std::forward<Arg0>(a0), param, str);

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
		using namespace ostr::literal;

		size_t prev_holder = 0;

		// TODO: performance hit here when construct string
		//	|	|	|	|	|	|	|	|
		//	v	v	v	v	v	v	v	v
		std::u16string ans;
		//	^	^	^	^	^	^	^	^
		//	|	|	|	|	|	|	|	|
		// TODO: performance hit here when construct string
		
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
				ans.append(fmt.substr(prev_holder, i - prev_holder - 1));
				size_t index = 0;
				size_t param_colon = SIZE_MAX;
				int alignment = 0;
				
				for (; i < fmt.size(); ++i)
				{
					c = fmt.data()[i];
					if (helper::character::is_number(c))
					{
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
							index,
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
						// ERROR!
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
