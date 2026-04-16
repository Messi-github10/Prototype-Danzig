#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <string>

namespace core {

class Logger {
public:
    static void init(bool console = true, const std::string& logFile = "");
    static std::shared_ptr<spdlog::logger> get() { return s_logger; }

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

// 便捷宏
#define LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(core::Logger::get(), __VA_ARGS__)
#define LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(core::Logger::get(), __VA_ARGS__)
#define LOG_INFO(...)     SPDLOG_LOGGER_INFO(core::Logger::get(), __VA_ARGS__)
#define LOG_WARNING(...)  SPDLOG_LOGGER_WARN(core::Logger::get(), __VA_ARGS__)
#define LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(core::Logger::get(), __VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(core::Logger::get(), __VA_ARGS__)

} // namespace core