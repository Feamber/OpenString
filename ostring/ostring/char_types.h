#pragma once
#include "definitions.h"

_NS_OSTR_BEGIN

using ansi_char = char;
using wide_char = wchar_t;

// Switchable between AnsiChar and WideChar
using char_t = wide_char;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;

using surrogate_pair = uint16_t[2];

enum class endian : uint8_t
{
	big,
	little
};

_NS_OSTR_END

