#pragma once
#include "definitions.h"
#include "string.h"

_NS_OSTR_BEGIN

inline ostr::string from_std_to_ostr(const std::string& std_str)
{
	return std_str.c_str();
}

inline ostr::string from_std_to_ostr(const std::wstring& std_wstr)
{
	return std_wstr.c_str();
}

// use std converter temporily
// use std converter temporily
// use std converter temporily

template<typename T>
inline ostr::string to_string(const T& variable)
{
	return variable.to_string();
}

template<>
inline ostr::string to_string(const int& int_variable)
{
	return from_std_to_ostr(std::to_wstring(int_variable));
}

template<>
inline ostr::string to_string(const float& float_variable)
{
	return from_std_to_ostr(std::to_wstring(float_variable));
}

template<>
inline ostr::string to_string(const double& double_variable)
{
	return from_std_to_ostr(std::to_wstring(double_variable));
}


_NS_OSTR_END
