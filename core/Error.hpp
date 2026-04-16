#pragma once

#include "Logger.hpp"
#include <string>
#include <stdexcept>

extern "C" {
#include <libavutil/error.h>
}

namespace core {

// 自定义异常类
class MediaException : public std::runtime_error {
public:
    MediaException(const std::string& what) : std::runtime_error(what) {}
};

class VulkanException : public std::runtime_error {
public:
    VulkanException(const std::string& what, int errorCode = 0)
        : std::runtime_error(what), m_errorCode(errorCode) {}

    int getErrorCode() const { return m_errorCode; }

private:
    int m_errorCode;
};

class SDLException : public std::runtime_error {
public:
    SDLException(const std::string& what) : std::runtime_error(what) {}
};



} // namespace core

// 错误检查宏（在core命名空间外部，使用完全限定名）
#define VK_CHECK(result, msg) \
    do { \
        VkResult vkResult = (result); \
        if (vkResult != VK_SUCCESS) { \
            LOG_ERROR("{}: VkResult = {}", msg, static_cast<int>(vkResult)); \
            throw core::VulkanException(std::string(msg) + ": VkResult = " + std::to_string(vkResult), vkResult); \
        } \
    } while(0)

#define SDL_CHECK(condition, msg) \
    do { \
        if (!(condition)) { \
            const char* sdlError = SDL_GetError(); \
            LOG_ERROR("{}: {}", msg, sdlError ? sdlError : "Unknown SDL error"); \
            throw core::SDLException(std::string(msg) + ": " + (sdlError ? sdlError : "Unknown SDL error")); \
        } \
    } while(0)

#define FFMPEG_CHECK(result, msg) \
    do { \
        int ffmpegResult = (result); \
        if (ffmpegResult < 0) { \
            char errorStr[AV_ERROR_MAX_STRING_SIZE]; \
            av_make_error_string(errorStr, AV_ERROR_MAX_STRING_SIZE, ffmpegResult); \
            LOG_ERROR("{}: {}", msg, errorStr); \
            throw core::MediaException(std::string(msg) + ": " + errorStr); \
        } \
    } while(0)

// 通用检查宏
#define CHECK(condition, msg) \
    do { \
        if (!(condition)) { \
            LOG_ERROR("Check failed: {}", msg); \
            throw std::runtime_error(std::string("Check failed: ") + msg); \
        } \
    } while(0)
