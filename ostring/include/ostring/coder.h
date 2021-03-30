#pragma once
#include "definitions.h"
#include <string>
#include <string_view>

_NS_OSTR_BEGIN

namespace coder {

	OPEN_STRING_EXPORT bool convert_append(std::string_view sv8, std::u16string& out_u16);

	OPEN_STRING_EXPORT bool convert_append(std::u16string_view sv16, std::string& out_u8);

	// not used temporarily
	/*bool convert_append(std::string_view sv8, std::u32string& out_u32)
	{

	}

	bool convert_append(std::u32string_view sv32, std::string& out_u8)
	{

	}

	bool convert_append(std::u16string_view sv16, std::u32string& out_u32)
	{

	}

	bool convert_append(std::u32string_view sv32, std::u16string& out_u16)
	{

	}*/
};

_NS_OSTR_END
