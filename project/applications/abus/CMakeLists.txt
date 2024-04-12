# CMakeList.txt: abus_v2.0.1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
# 将整个文件夹添加到项目
file(GLOB_RECURSE SRC_FILES "*.cpp" "*.c" "*.h")
add_executable (abus_v2.0.1 ${SRC_FILES})

# add_executable (abus_v2.0.1 "abus_v2.0.1.cpp" "abus_v2.0.1.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET abus_v2.0.1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
