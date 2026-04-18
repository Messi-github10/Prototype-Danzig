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

# GoogleTest configuration - try system package first, then FetchContent
if(ENABLE_TESTS)
    # 先尝试查找系统安装的GoogleTest
    find_package(GTest QUIET)

    if(NOT GTest_FOUND)
        message(STATUS "GTest not found, downloading via FetchContent...")

        # 使用FetchContent自动下载
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.14.0
        )
        FetchContent_MakeAvailable(googletest)

        message(STATUS "GTest will be built from source")
    else()
        message(STATUS "Using system GTest: ${GTest_VERSION}")
    endif()
endif()

# 添加子目录
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(audio)
add_subdirectory(media)
add_subdirectory(render)
add_subdirectory(sync)

# 主可执行文件
add_executable(Prototype-Danzig main.cpp)

# 链接库
target_link_libraries(Prototype-Danzig PRIVATE
    core
    Vulkan::Vulkan
    volk::volk
    PkgConfig::FFMPEG
)

# 包含目录
target_include_directories(Prototype-Danzig PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}  # 根目录
    ${Vulkan_INCLUDE_DIRS}
    ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include
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
    add_subdirectory(tests)
endif()
