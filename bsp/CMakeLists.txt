# 1. BSP板级支持包源文件
#----------------------------------------
# 1.1 评估板相关源文件
set(BSP_BOARD_SOURCES
    ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2_24cxx.c
    ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2_w25qxx.c
    ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2_wm8731.c
    ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2.c
)

# 1.2 组件源文件
set(BSP_COMPONENT_SOURCES
    components/24cxx/24cxx.c
    components/malloc/malloc.c
    components/malloc/memory_management.cpp
    components/w25qxx/w25qxx.c
    components/wm8731/wm8731.c
)

# 1.3 外设底层配置文件
set(BSP_PERIPHERAL_SOURCES
    peripherals/can/bspCan.c
    peripherals/cpu_utilization/cpuUtilization.c
    peripherals/eirq/bspEirq.c
    peripherals/led/bspLed.cpp
    peripherals/pwm/bspPwm.cpp
    peripherals/uart/bspUart4.c
    peripherals/uart/printfUart.c
)

# 2. 合并所有源文件
#----------------------------------------
set(BSP_SOURCES
    ${BSP_BOARD_SOURCES}
    ${BSP_COMPONENT_SOURCES}
    ${BSP_PERIPHERAL_SOURCES}
)

# 3. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${BSP_SOURCES})

# 4. 包含目录设置
#----------------------------------------
# 4.1 BSP头文件路径
set(BSP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/components/24cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/components/malloc
    ${CMAKE_CURRENT_SOURCE_DIR}/components/w25qxx
    ${CMAKE_CURRENT_SOURCE_DIR}/components/wm8731
    ${CMAKE_CURRENT_SOURCE_DIR}/ev_hc32f460_lqfp100_v2
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/can
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/cpu_utilization
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/eirq
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/led
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/pwm
    ${CMAKE_CURRENT_SOURCE_DIR}/peripherals/uart
)

# 5. 添加所有包含路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BSP_INCLUDE_DIRS}
)
