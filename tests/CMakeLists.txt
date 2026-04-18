# tests模块
if(NOT TARGET core)
    message(FATAL_ERROR "core target not found, tests require core module")
endif()

# 添加测试可执行文件
add_executable(test_core
    test_core.cpp
)

# 包含目录
target_include_directories(test_core PRIVATE
    ${CMAKE_SOURCE_DIR}  # 根目录，用于包含core头文件
)

# 链接依赖库
target_link_libraries(test_core PRIVATE
    core
    GTest::gtest
    GTest::gtest_main
)

# 添加测试
add_test(NAME core_tests COMMAND test_core)