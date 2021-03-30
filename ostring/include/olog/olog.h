#pragma once
#include "definitions.h"
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <ostring/osv.h>
#include <ostring/helpers.h>


_NS_OLOG_START

#include <Windows.h>
struct spdlog_sys
{
    static void init_log_system()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);
        console_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            "Daily.log",        // file name
            0,                  // hour
            0,                  // minute
            false,              // truncate
            7                   // max files
        );
        file_sink->set_level(spdlog::level::trace);

        auto logger = std::make_shared<spdlog::logger>("logger_default", spdlog::sinks_init_list{ console_sink, file_sink });
        logger->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);
    }

    static void verbose(ostr::string_view sv)
    {
        //spdlog::trace(sv);
    }

    static void critical(ostr::string_view sv)
    {
        std::string str;
        sv.to_utf8(str);
        spdlog::critical(str);
    }

};

using logsys = spdlog_sys;

void init_log_system()
{
    logsys::init_log_system();
}

void verbose(ostr::string_view sv)
{
    logsys::verbose(sv);
}

void critical(ostr::string_view sv)
{
    logsys::critical(sv);
}

_NS_OLOG_END
