#pragma once
#include "definitions.h"
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <ostring/ostr.h>
#include <ostring/osv.h>
#include <ostring/helpers.h>


_NS_OLOG_START

#include <Windows.h>
struct spdlog_sys
{

    static void init_log_system()
    {
        auto console_sink = get_console_sink();
        console_sink->set_level(spdlog::level::info);
        console_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

        auto file_sink = get_file_sink();
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern("[%Y-%m-%d %z %X.(%F)] [%^%l%$] [%P.%t] %v");

        auto logger = std::make_shared<spdlog::logger>("logger_default", spdlog::sinks_init_list{ console_sink, file_sink });
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::info);
        spdlog::set_default_logger(logger);
    }

    static void reset_pattern()
    {
        get_console_sink()->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
        get_file_sink()->set_pattern("[%Y-%m-%d %z %X.(%F)] [%^%l%$] [%P.%t] %v");


        std::string_view* pfile;
        std::string_view* pfunc;
        int* pline;
        bool& set = get_ffl(pfile, pfunc, pline);
        set = false;
    }

    static void __log_with_ffl(std::string_view file, std::string_view func, int line)
    {
        std::string_view* pfile;
        std::string_view* pfunc;
        int* pline;
        bool& set = get_ffl(pfile, pfunc, pline);
        set = true;
        *pfile = file;
        *pfunc = func;
        *pline = line;
    }

    static void verbose(std::string_view sv)
    {
        change_pattern_if_ffl();
        spdlog::trace(sv);
        reset_pattern();
    }

    static void debug(std::string_view sv)
    {
        change_pattern_if_ffl();
        spdlog::debug(sv);
        reset_pattern();
    }

    static void info(std::string_view sv)
    {
        change_pattern_if_ffl();
        spdlog::info(sv);
        reset_pattern();
    }

    static void warn(std::string_view sv)
    {
        change_pattern_if_ffl();
        spdlog::warn(sv);
        reset_pattern();
    }

    static void error(std::string_view sv)
    {
        change_pattern_if_ffl();
        spdlog::error(sv);
        reset_pattern();
    }

    static void fatal(std::string_view sv)
    {
        change_pattern_if_ffl();
        spdlog::critical(sv);
        reset_pattern();
    }

private:

    static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> get_console_sink()
    {
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> _static = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        return _static;
    }

    static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> get_file_sink()
    {
        static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> _static = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            "Daily.log",        // file name
            0,                  // hour
            0,                  // minute
            false,              // truncate
            7                   // max files
            );
        return _static;
    }

    static bool& get_ffl(std::string_view*& file, std::string_view*& func, int*& line)
    {
        static std::string_view _s_file;
        static std::string_view _s_func;
        static int _s_line;
        static bool _s_set;
        file = &_s_file;
        func = &_s_func;
        line = &_s_line;
        return _s_set;
    }

    static void change_pattern_if_ffl()
    {
        std::string_view* pfile;
        std::string_view* pfunc;
        int* pline;
        bool set = get_ffl(pfile, pfunc, pline);
        if (set)
        {
            const std::string pattern_console = "[%H:%M:%S.%e] [%^%l%$] ";
            const std::string file_console = "[%Y-%m-%d %z %X.(%F)] [%^%l%$] [%P.%t] ";
            std::string ffl = "[";
            ffl += *pfunc;
            ffl += " @ ";
            ffl += *pfile;
            ffl += ": ";
            ffl += std::to_string(*pline);
            ffl += "] ";
            get_console_sink()->set_pattern(pattern_console + ffl + "%v");
            get_file_sink()->set_pattern(file_console + ffl + "%v");
        }
    }
};

// may bring in UE or unity
using logsys = spdlog_sys;
#define LOG_FFL olog::spdlog_sys::__log_with_ffl(__FILE__, __FUNCTION__, __LINE__);


void init_log_system()
{
    logsys::init_log_system();
}

#define __MAKE_LOG_CALL_DECLARE(func)\
void func(std::string_view sv)\
{\
    logsys::func(sv);\
}\
void func(const std::string& sv)\
{\
    func(std::string_view(sv));\
}\
void func(ostr::string_view sv)\
{\
    std::string str;\
    sv.to_utf8(str);\
    func(std::string_view(str));\
}\
void func(const ostr::string& sv)\
{\
    func(ostr::string_view(sv));\
}\
void func(const std::u16string& str)\
{\
    func(ostr::string_view(str));\
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
