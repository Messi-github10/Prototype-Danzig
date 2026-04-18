# common模块 - 头文件库
add_library(common INTERFACE)

# 包含当前目录
target_include_directories(common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置C++标准
target_compile_features(common INTERFACE cxx_std_20)

# 传递编译定义（如果有）
# target_compile_definitions(common INTERFACE ...)