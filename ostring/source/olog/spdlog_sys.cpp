#include "olog/spdlog_sys.h"
#include <Windows.h>

_NS_OLOG_START

void spdlog_sys::init_log_system()
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

    SetConsoleOutputCP(65001);
}

void spdlog_sys::reset_pattern()
{
    get_console_sink()->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
    get_file_sink()->set_pattern("[%Y-%m-%d %z %X.(%F)] [%^%l%$] [%P.%t] %v");


    std::string_view* pfile;
    std::string_view* pfunc;
    int* pline;
    bool& set = get_ffl(pfile, pfunc, pline);
    set = false;
}

void spdlog_sys::__log_with_ffl(std::string_view file, std::string_view func, int line)
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

void spdlog_sys::verbose(std::wstring_view sv)
{
    change_pattern_if_ffl();
    spdlog::trace(sv);
    reset_pattern();
}

void spdlog_sys::debug(std::wstring_view sv)
{
    change_pattern_if_ffl();
    spdlog::debug(sv);
    reset_pattern();
}

void spdlog_sys::info(std::wstring_view sv)
{
    change_pattern_if_ffl();
    spdlog::info(sv);
    reset_pattern();
}

void spdlog_sys::warn(std::wstring_view sv)
{
    change_pattern_if_ffl();
    spdlog::warn(sv);
    reset_pattern();
}

void spdlog_sys::error(std::wstring_view sv)
{
    change_pattern_if_ffl();
    spdlog::error(sv);
    reset_pattern();
}

void spdlog_sys::fatal(std::wstring_view sv)
{
    change_pattern_if_ffl();
    spdlog::critical(sv);
    reset_pattern();
}

std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> spdlog_sys::get_console_sink()
{
    static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> _static = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    return _static;
}

std::shared_ptr<spdlog::sinks::daily_file_sink_mt> spdlog_sys::get_file_sink()
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

bool& spdlog_sys::get_ffl(std::string_view*& file, std::string_view*& func, int*& line)
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

void spdlog_sys::change_pattern_if_ffl()
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

_NS_OLOG_END
