#include "core/Logger.hpp"
#include "core/Error.hpp"
#include <iostream>
#include <limits>

// SDL2
#include <SDL2/SDL.h>

// Vulkan - volk provides Vulkan functions
#include <volk.h>

// FFmpeg
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

void testSpdlog() {
    LOG_INFO("=== Testing spdlog ===");
    LOG_TRACE("This is a trace message");
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARNING("This is a warning message");
    LOG_ERROR("This is an error message");
    LOG_INFO("spdlog test completed");
}

void testSDL() {
    LOG_INFO("=== Testing SDL2 ===");
    SDL_CHECK(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0, "SDL_Init failed");
    LOG_INFO("SDL2 initialized successfully");

    // 获取SDL版本
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    LOG_INFO("SDL Compiled: {}.{}.{}", compiled.major, compiled.minor, compiled.patch);
    LOG_INFO("SDL Linked: {}.{}.{}", linked.major, linked.minor, linked.patch);

    SDL_Quit();
    LOG_INFO("SDL2 quit successfully");
}

void testVulkan() {
    LOG_INFO("=== Testing Vulkan ===");

    VkResult result = volkInitialize();
    VK_CHECK(result, "volkInitialize failed");
    LOG_INFO("Volk initialized successfully");

    // 创建Vulkan实例
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Prototype Danzig Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    result = vkCreateInstance(&createInfo, nullptr, &instance);
    VK_CHECK(result, "vkCreateInstance failed");
    LOG_INFO("Vulkan instance created successfully");

    // 加载实例函数
    volkLoadInstance(instance);

    // 枚举物理设备
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    LOG_INFO("Number of Vulkan physical devices: {}", deviceCount);

    vkDestroyInstance(instance, nullptr);
    LOG_INFO("Vulkan instance destroyed");
}

void testFFmpeg() {
    LOG_INFO("=== Testing FFmpeg ===");

    avformat_network_init();
    LOG_INFO("FFmpeg network initialized");

    // 检查版本
    unsigned avcodec_version_num = avcodec_version();
    LOG_INFO("avcodec version: {}.{}.{}",
             avcodec_version_num >> 16,
             (avcodec_version_num >> 8) & 0xff,
             avcodec_version_num & 0xff);

    const char* configure = avcodec_configuration();
    LOG_INFO("avcodec configuration: {}", configure ? configure : "N/A");

    // 列出部分编解码器
    void* opaque = nullptr;
    const AVCodec* codec = nullptr;
    LOG_INFO("Available codecs (first 5):");
    int count = 0;
    while ((codec = av_codec_iterate(&opaque)) != nullptr && count < 5) {
        std::string capabilities;
        if (av_codec_is_decoder(codec)) {
            capabilities += "decoder";
            if (av_codec_is_encoder(codec)) {
                capabilities += ", encoder";
            }
        } else if (av_codec_is_encoder(codec)) {
            capabilities += "encoder";
        }

        LOG_INFO("  {} ({})", codec->name, capabilities);
        count++;
    }

    avformat_network_deinit();
    LOG_INFO("FFmpeg network deinitialized");
}

int main() {
    try {
        // 初始化日志系统
        core::Logger::init(true, "prototype_danzig.log");

        LOG_INFO("=== Prototype Danzig Starting ===");
        LOG_INFO("Build: {} {}", __DATE__, __TIME__);

        // 测试各个库
        testSpdlog();
        testSDL();
        testVulkan();
        testFFmpeg();

        LOG_INFO("=== All libraries tested successfully! ===");
        LOG_INFO("Press Enter to exit...");

        // 清空输入缓冲区，然后等待回车
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();

        LOG_INFO("=== Shutting down ===");

    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}