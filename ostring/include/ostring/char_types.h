#pragma once
#include "definitions.h"

_NS_OSTR_BEGIN

using char8_t = unsigned char;

using surrogate_pair = char16_t[2];
using utf8_sequence = char8_t[4];

enum class endian : uint8_t
{
	big,
	little
};

enum class case_sensitivity : uint8_t
{
	sensitive,
	insensitive
};

_NS_OSTR_END

