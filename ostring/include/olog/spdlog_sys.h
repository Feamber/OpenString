#pragma once
#include "definitions.h"
#include <string_view>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

_NS_OLOG_START

struct spdlog_sys
{

    static void init_log_system();

    static void reset_pattern();

    static void __log_with_ffl(std::string_view file, std::string_view func, int line);

    static void verbose(std::wstring_view sv);

    static void debug(std::wstring_view sv);

    static void info(std::wstring_view sv);

    static void warn(std::wstring_view sv);

    static void error(std::wstring_view sv);

    static void fatal(std::wstring_view sv);

private:

    static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> get_console_sink();

    static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> get_file_sink();

    static bool& get_ffl(std::string_view*& file, std::string_view*& func, int*& line);

    static void change_pattern_if_ffl();
};

_NS_OLOG_END
