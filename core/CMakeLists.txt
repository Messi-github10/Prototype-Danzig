# core模块
add_library(core
    Logger.cpp
    Logger.hpp
    Error.hpp
    Window.cpp
    Window.hpp
)

# 包含目录
target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}  # 根目录，用于包含core/头文件
)

# 链接依赖库
target_link_libraries(core PUBLIC
    spdlog::spdlog
    SDL2::SDL2
)

# 链接common模块（如果存在）
if(TARGET common)
    target_link_libraries(core PRIVATE common)
endif()

# C++标准
target_compile_features(core PUBLIC cxx_std_20)

# 编译定义
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(core PRIVATE
        DEBUG_BUILD
        SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
    )
    if(ENABLE_DEBUG_LAYERS)
        target_compile_definitions(core PRIVATE
            ENABLE_VULKAN_VALIDATION_LAYERS
        )
    endif()
else()
    target_compile_definitions(core PRIVATE
        RELEASE_BUILD
        SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO
    )
endif()

# 安装目标（可选）
install(TARGETS core DESTINATION lib)
install(FILES Logger.hpp Error.hpp Window.hpp DESTINATION include/core)