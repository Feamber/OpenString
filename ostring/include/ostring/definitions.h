#pragma once

#define _NS_OSTR_BEGIN	namespace ostr{
#define _NS_OSTR_END	}

#define OSTR(x) u ## x ## o
#define OCHAR(x) u ## x

#define OPEN_STRING_EXPORT __declspec(dllexport)
