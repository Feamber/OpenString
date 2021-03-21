#pragma once
#include "definitions.h"

_NS_OSTR_BEGIN

using ansi_char = char;
using wide_char = wchar_t;

using char8_t = unsigned char;

using surrogate_pair = char16_t[2];

enum class endian : char8_t
{
	big,
	little
};

enum class case_sensitivity : char8_t
{
	sensitive,
	insensitive
};

_NS_OSTR_END

