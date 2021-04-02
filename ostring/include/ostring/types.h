#pragma once
#include "definitions.h"

_NS_OSTR_BEGIN

using char8_t = unsigned char;

using utf8_sequence = char8_t[4];
using surrogate_pair = char16_t[2];
using codepoint = char32_t;

using small_size_t = unsigned short;

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

