#pragma once
#include "definitions.h"
#include <string>
#include "ostring/ostr.h"
#include "ostring/osv.h"
#include "spdlog_sys.h"

// TODO: platform independent
// TODO: move impl to cpp
_NS_OLOG_START

// may bring in UE or unity
using logsys = spdlog_sys;

// mark for logging [function @ file : line] in next single log
#define LOG_FFL olog::spdlog_sys::__log_with_ffl(__FILE__, __FUNCTION__, __LINE__);

void init_log_system();

#define __MAKE_LOG_CALL_DECLARE(func)\
void func(std::wstring_view sv);\
void func(ostr::string_view sv);\
void func(const ostr::string& sv);\
void func(const std::u16string& str);\
void func(const std::u16string_view& str);\
template<typename T, typename _Char = typename std::char_traits<T>::char_type>\
void func(_Char const* cstr)\
{\
    func(std::wstring_view(std::to_wstring(cstr)));\
}\
template<typename T>\
void func(std::basic_string_view<T> sv)\
{\
    std::u16string str(sv.cbegin(), sv.cend());\
    func(std::u16string_view(str));\
}\
template<typename T>\
void func(std::basic_string<T> str)\
{\
    func(std::basic_string_view<T>(str));\
}\
template<typename T, typename...Args>\
void func(T fmt, Args&&...args)\
{\
    func(fmt::format(fmt, std::forward<Args>(args)...));\
}

__MAKE_LOG_CALL_DECLARE(verbose)
__MAKE_LOG_CALL_DECLARE(debug)
__MAKE_LOG_CALL_DECLARE(info)
__MAKE_LOG_CALL_DECLARE(warn)
__MAKE_LOG_CALL_DECLARE(error)
__MAKE_LOG_CALL_DECLARE(fatal)


#undef __MAKE_LOG_CALL_DECLARE

_NS_OLOG_END
