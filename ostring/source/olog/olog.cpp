#include "olog/olog.h"

_NS_OLOG_START

void init_log_system()
{
    logsys::init_log_system();
}

#define __MAKE_LOG_CALL_IMPL(func)\
void func(std::wstring_view sv)\
{\
    logsys::func(sv);\
}\
void func(ostr::string_view sv)\
{\
    std::wstring str(sv.raw().cbegin(), sv.raw().cend());\
    func(std::wstring_view(str));\
}\
void func(const ostr::string& sv)\
{\
    func(ostr::string_view(sv));\
}\
void func(const std::u16string& str)\
{\
    func(ostr::string_view(str));\
}\
void func(const std::u16string_view& str)\
{\
    func(ostr::string_view(str));\
}

__MAKE_LOG_CALL_IMPL(verbose)
__MAKE_LOG_CALL_IMPL(debug)
__MAKE_LOG_CALL_IMPL(info)
__MAKE_LOG_CALL_IMPL(warn)
__MAKE_LOG_CALL_IMPL(error)
__MAKE_LOG_CALL_IMPL(fatal)

#undef __MAKE_LOG_CALL_IMPL

_NS_OLOG_END
