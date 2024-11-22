# 1. CMake基础设置
#----------------------------------------
cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 2. 工具链和项目设置
#----------------------------------------
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")
project(HC32F460_CMake_Template LANGUAGES C CXX ASM)

# 3. 构建类型设置
#----------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# 4. 输出路径设置
#----------------------------------------
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 5. 输出文件设置
#----------------------------------------
set(TARGET_ELF ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf)
set(TARGET_HEX ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex)
set(TARGET_BIN ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin)
set(MAP_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map)

# 6. 目标创建和源文件添加，以及设置目标的宏定义
#----------------------------------------
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${STARTUP_FILE})
target_compile_definitions(${PROJECT_NAME} PRIVATE
    __DEBUG
    HC32F460
    USE_DDL_DRIVER
)

# 7. 子目录添加
#----------------------------------------
add_subdirectory(configs)
add_subdirectory(core)
add_subdirectory(drivers)
add_subdirectory(bsp)
add_subdirectory(hal)
add_subdirectory(libraries)
add_subdirectory(middlewares)
add_subdirectory(source)
add_subdirectory(application)

# 8. 后期构建命令
#----------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${TARGET_ELF}
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${TARGET_HEX}
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${TARGET_BIN}
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)