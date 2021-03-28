#pragma once
#include "string.h"
#include "string_view.h"

_NS_OSTR_BEGIN

// format string
// format rule: fmtlib @ https://github.com/fmtlib/fmt
/*template<typename...Args>
string format(const char* fmt, Args&&...args)
{
	return format(string_view(fmt), std::forward<Args>(args)...);
}*/

// format string
// format rule: fmtlib @ https://github.com/fmtlib/fmt
template<typename...Args>
string format(const char16_t* fmt, Args&&...args)
{
	return std::move( format(string_view(fmt), std::forward<Args>(args)...) );
}

// format string
// format rule: fmtlib @ https://github.com/fmtlib/fmt
template<typename...Args>
string format(const string_view& fmt, Args&&...args)
{
	// return format<Args...>(fmt.to_string(), std::forward<Args>(args)...);
	return fmt.format(std::forward<Args>(args)...);
}

template<typename...Args>
string format(const string& fmt, Args&&...args)
{
	// return fmt.format(std::forward<Args>(args)...);
	return format<Args...>(string_view( fmt ), std::forward<Args>(args)...);
}

_NS_OSTR_END
