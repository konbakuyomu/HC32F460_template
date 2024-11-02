# 1. CMake基础设置
#----------------------------------------
cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 2. 工具链和目标系统设置
#----------------------------------------
# 设置工具链文件
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")
# 设置项目名称和支持的语言
project(HC32F460_CMake_Template LANGUAGES C CXX ASM)

# 3. 构建类型设置
#----------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# 4. 全局定义和编译选项
#----------------------------------------
add_definitions(
    -D__DEBUG
    -DHC32F460
    -DUSE_DDL_DRIVER
)

# 5. 输出路径设置
#----------------------------------------
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 6. 关键文件路径设置
#----------------------------------------
# 设置启动文件和链接脚本
set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/drivers/cmsis/Device/HDSC/hc32f4xx/Source/GCC/startup_hc32f460.S)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/drivers/cmsis/Device/HDSC/hc32f4xx/Source/GCC/linker/HC32F460xE.ld)

# 7. 输出文件设置
#----------------------------------------
set(TARGET_ELF ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf)
set(TARGET_HEX ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex)
set(TARGET_BIN ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map)

# 8. 目标创建和源文件添加
#----------------------------------------
# 创建可执行文件目标
add_executable(${PROJECT_NAME})
# 添加启动文件
target_sources(${PROJECT_NAME} PRIVATE ${STARTUP_FILE})

# 9. 包含目录设置
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 10. 子目录添加
#----------------------------------------
add_subdirectory(drivers)
add_subdirectory(middlewares)
add_subdirectory(source)

# 11. 编译选项设置
#----------------------------------------
target_compile_options(${PROJECT_NAME} PRIVATE
    # MCU 相关选项
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    
    # 优化相关选项
    -ffunction-sections
    -fdata-sections
    -fno-common
    -fno-builtin
    
    # 警告相关选项
    -Wall
    -Wextra
)

# 12. 链接选项设置
#----------------------------------------
target_link_options(${PROJECT_NAME} PRIVATE
    # MCU 相关选项
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    
    # 链接器脚本
    -T${LINKER_SCRIPT}
    
    # 链接器特定选项
    "LINKER:--gc-sections"
    "LINKER:--no-warn-rwx-segments"
    "LINKER:-Map=${MAP_FILE}"
    "LINKER:--print-memory-usage"
    
    # 系统库相关选项
    -specs=nano.specs
    -specs=nosys.specs
    -lm
)

# 13. 后期构建命令
#----------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # 复制 ELF 文件
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${TARGET_ELF}
    # 生成 HEX 文件
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${TARGET_HEX}
    # 生成 BIN 文件
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${TARGET_BIN}
    # 打印大小信息
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)