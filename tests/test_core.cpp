#include "core/Logger.hpp"
#include "core/Error.hpp"
#include <iostream>

void testLogger() {
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

void testErrorMacros() {
    std::cout << "Testing error macros (compile-time check)..." << std::endl;

    // 这些宏应该能编译通过
    // 实际错误检查将在运行时测试

    // 测试通用CHECK宏
    CHECK(true, "This should not throw");

    std::cout << "Error macro compilation check passed!" << std::endl;
}

void testExceptions() {
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

int main() {
    std::cout << "=== Running core module tests ===" << std::endl;

    try {
        testLogger();
        testErrorMacros();
        testExceptions();

        std::cout << "=== All core tests passed! ===" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}