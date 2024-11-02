# 设置尝试编译的目标类型为静态库
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 设置目标系统名称和处理器
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

# 设置ARM GCC编译器的根路径
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")
set(CMAKE_SIZE "arm-none-eabi-size")

# 强制使用指定的编译器
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# 设置查找模式
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 禁用编译器检查
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# 设置通用编译器选项
set(COMMON_FLAGS
    "-mcpu=cortex-m4 \
     -mthumb \
     -mfpu=fpv4-sp-d16 \
     -mfloat-abi=hard \
     -ffunction-sections \
     -fdata-sections \
     -fno-common \
     -fno-builtin \
     -Wall \
     -Wextra"
)

# 设置C语言特定选项
set(CMAKE_C_FLAGS "${COMMON_FLAGS} \
    -std=c11 \
    -Wno-main \
    -Wno-unused-function \
    -Wno-missing-prototypes \
    -Wno-unused-parameter"
)

# 设置C++语言特定选项
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} \
    -std=c++14 \
    -Wno-main \
    -Wno-unused-function \
    -Wno-unused-parameter \
    -Wno-missing-field-initializers"
)

# 根据构建类型设置优化级别
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g3")
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
endif()