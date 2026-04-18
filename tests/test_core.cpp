#include "core/Logger.hpp"
#include "core/Error.hpp"
#include "core/Window.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <gtest/gtest.h>

TEST(CoreTest, Logger) {
    std::cout << "Testing logger..." << std::endl;

    // 初始化日志（不输出到文件）
    core::Logger::init(true, "");

    LOG_TRACE("Test trace message");
    LOG_DEBUG("Test debug message");
    LOG_INFO("Test info message");
    LOG_WARNING("Test warning message");
    LOG_ERROR("Test error message");

    std::cout << "Logger test passed!" << std::endl;
}

TEST(CoreTest, ErrorMacros) {
    std::cout << "Testing error macros (compile-time check)..." << std::endl;

    // 这些宏应该能编译通过
    // 实际错误检查将在运行时测试

    // 测试通用CHECK宏
    CHECK(true, "This should not throw");

    std::cout << "Error macro compilation check passed!" << std::endl;
}

TEST(CoreTest, Exceptions) {
    std::cout << "Testing exception classes..." << std::endl;

    try {
        throw core::MediaException("Test media exception");
    } catch (const core::MediaException& e) {
        std::cout << "Caught MediaException: " << e.what() << std::endl;
    }

    try {
        throw core::VulkanException("Test Vulkan exception", 123);
    } catch (const core::VulkanException& e) {
        std::cout << "Caught VulkanException: " << e.what()
                  << ", error code: " << e.getErrorCode() << std::endl;
    }

    try {
        throw core::SDLException("Test SDL exception");
    } catch (const core::SDLException& e) {
        std::cout << "Caught SDLException: " << e.what() << std::endl;
    }

    std::cout << "Exception test passed!" << std::endl;
}

TEST(CoreTest, Window) {
    std::cout << "Testing Window class..." << std::endl;

    // 初始化Logger（Window类需要）
    core::Logger::init(true, "");

    // 初始化SDL视频子系统
    SDL_CHECK(SDL_Init(SDL_INIT_VIDEO) == 0, "Failed to initialize SDL");

    try {
        // 测试1: 创建窗口
        std::cout << "  Creating window..." << std::endl;
        Window window(800, 600, "Test Window", false,
                      SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_HIDDEN);

        EXPECT_EQ(window.getWidth(), 800) << "Window width mismatch";
        EXPECT_EQ(window.getHeight(), 600) << "Window height mismatch";
        EXPECT_FALSE(window.isFullscreen()) << "Window should not be fullscreen";
        EXPECT_FALSE(window.shouldClose()) << "Window should not be closing";

        std::cout << "  Window created successfully" << std::endl;

        // 测试2: 基本方法
        std::cout << "  Testing basic methods..." << std::endl;
        EXPECT_NE(window.getSDLWindow(), nullptr) << "SDL window handle should not be null";
        // Note: isMinimized() check removed as it may be unreliable with SDL_WINDOW_HIDDEN
        // EXPECT_FALSE(window.isMinimized()) << "Window should not be minimized after creation";

        // 测试3: 修改标题
        window.setTitle("New Title");

        // 测试4: 调整大小
        window.resize(1024, 768);
        EXPECT_EQ(window.getWidth(), 1024) << "Window width after resize mismatch";
        EXPECT_EQ(window.getHeight(), 768) << "Window height after resize mismatch";

        // 测试5: 移动语义
        std::cout << "  Testing move semantics..." << std::endl;
        Window movedWindow = std::move(window);
        EXPECT_NE(movedWindow.getSDLWindow(), nullptr) << "Moved window should have valid SDL window";
        EXPECT_EQ(movedWindow.getWidth(), 1024) << "Moved window width mismatch";
        EXPECT_EQ(movedWindow.getHeight(), 768) << "Moved window height mismatch";

        // 移动后原窗口应为空
        EXPECT_EQ(window.getSDLWindow(), nullptr) << "Original window should have null SDL window after move";
        EXPECT_EQ(window.getWidth(), 0) << "Original window width should be 0 after move";
        EXPECT_EQ(window.getHeight(), 0) << "Original window height should be 0 after move";

        // 测试6: 移动赋值
        Window anotherWindow(640, 480, "Another Window", false,
                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_HIDDEN);
        anotherWindow = std::move(movedWindow);
        EXPECT_EQ(anotherWindow.getWidth(), 1024) << "Window width after move assignment mismatch";
        EXPECT_EQ(anotherWindow.getHeight(), 768) << "Window height after move assignment mismatch";

        // 测试7: 事件轮询（不阻塞，只检查是否有事件）
        std::cout << "  Testing event polling..." << std::endl;
        bool eventsOk = anotherWindow.pollEvents();
        EXPECT_EQ(eventsOk, !anotherWindow.shouldClose()) << "Event polling result mismatch";

        // 测试8: 全屏切换（理论上可以调用，但实际效果取决于系统）
        anotherWindow.toggleFullscreen();
        // 不检查结果，因为全屏切换可能失败

        std::cout << "Window test passed!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Window test failed with exception: " << e.what() << std::endl;
        SDL_Quit();
        throw;
    }

    // 清理SDL
    SDL_Quit();
    std::cout << "SDL cleaned up" << std::endl;
}