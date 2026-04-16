cmake_minimum_required(VERSION 3.16)
project(Prototype-Danzig LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 构建类型设置
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug/Release/RelWithDebInfo)")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")

# VCPKG integration
set(VCPKG_ROOT "/home/nicefold/Code/Github/vcpkg" CACHE PATH "VCPKG root directory")
set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "VCPKG target triplet")

# 添加vcpkg cmake模块到搜索路径
list(PREPEND CMAKE_PREFIX_PATH
    ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/share
    ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}
)

# 查找所有依赖
find_package(SDL2 REQUIRED CONFIG)
find_package(Vulkan REQUIRED COMPONENTS glslc)  # 包含glslc编译器
find_package(volk REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)  # spdlog日志库
# find_package(vma REQUIRED CONFIG)  # Vulkan Memory Allocator - header-only library

# FFmpeg使用pkg-config
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib/pkgconfig")
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
)

# 项目配置
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_DEBUG_LAYERS "Enable Vulkan debug layers in Debug mode" ON)

# 收集所有源文件（排除tests目录和构建目录）
file(GLOB_RECURSE ALL_SRC_FILES
    "*.cpp"
    "core/*.cpp"
    "media/*.cpp"
    "render/*.cpp"
    "audio/*.cpp"
    "sync/*.cpp"
)

# 过滤掉测试文件和构建目录文件
set(SRC_FILES ""
        core/Window.cpp
        core/Window.hpp)
foreach(file ${ALL_SRC_FILES})
    # 排除tests目录中的文件
    if(NOT file MATCHES "/tests/")
        # 排除构建目录中的文件
        if(NOT file MATCHES "/cmake-build-debug/" AND NOT file MATCHES "/build/")
            list(APPEND SRC_FILES ${file})
        endif()
    endif()
endforeach()

# 主可执行文件
add_executable(Prototype-Danzig ${SRC_FILES})

# 包含目录
target_include_directories(Prototype-Danzig PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}  # 根目录
    ${Vulkan_INCLUDE_DIRS}
    ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include
)

# 链接所有库
target_link_libraries(Prototype-Danzig PRIVATE
    SDL2::SDL2
    Vulkan::Vulkan
    volk::volk
    spdlog::spdlog
    # vma::vma  # Vulkan Memory Allocator - header-only library
    PkgConfig::FFMPEG
)

# 编译器设置
target_compile_features(Prototype-Danzig PRIVATE cxx_std_20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Prototype-Danzig PRIVATE
        DEBUG_BUILD
        SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
    )
    if(ENABLE_DEBUG_LAYERS)
        target_compile_definitions(Prototype-Danzig PRIVATE
            ENABLE_VULKAN_VALIDATION_LAYERS
        )
    endif()
else()
    target_compile_definitions(Prototype-Danzig PRIVATE
        RELEASE_BUILD
        SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO
    )
endif()

# 安装目标（可选）
install(TARGETS Prototype-Danzig DESTINATION bin)

# 测试配置
if(ENABLE_TESTS)
    enable_testing()

    # 收集core模块源文件
    file(GLOB_RECURSE CORE_SRC_FILES "core/*.cpp")
    file(GLOB_RECURSE TEST_SRC_FILES "tests/*.cpp")

    add_executable(test_core ${TEST_SRC_FILES} ${CORE_SRC_FILES})
    target_include_directories(test_core PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include
    )
    target_link_libraries(test_core PRIVATE
        spdlog::spdlog
        PkgConfig::FFMPEG  # 提供FFmpeg头文件路径
    )
    add_test(NAME core_tests COMMAND test_core)
endif()
