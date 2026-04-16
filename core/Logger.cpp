#include "Logger.hpp"

namespace core {

std::shared_ptr<spdlog::logger> Logger::s_logger = nullptr;

void Logger::init(bool console, const std::string& logFile) {
    std::vector<spdlog::sink_ptr> sinks;

    if (console) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
        sinks.push_back(console_sink);
    }

    if (!logFile.empty()) {
        try {
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
            sinks.push_back(file_sink);
        } catch (const spdlog::spdlog_ex& ex) {
            spdlog::error("Failed to create log file {}: {}", logFile, ex.what());
        }
    }

    if (sinks.empty()) {
        // 如果没有sink，创建一个控制台sink但设置级别为off
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::off);
        sinks.push_back(console_sink);
    }

    s_logger = std::make_shared<spdlog::logger>("mini_media_player", sinks.begin(), sinks.end());
    s_logger->set_level(spdlog::level::trace);
    s_logger->flush_on(spdlog::level::err);

    // 注册为默认logger
    spdlog::set_default_logger(s_logger);
}

} // namespace core