# 1. HC32 LL驱动源文件
#----------------------------------------
set(HC32_LL_SOURCES
    hc32_ll_driver/src/hc32_ll_adc.c
    hc32_ll_driver/src/hc32_ll_aes.c
    hc32_ll_driver/src/hc32_ll_aos.c
    hc32_ll_driver/src/hc32_ll_can.c
    hc32_ll_driver/src/hc32_ll_clk.c
    hc32_ll_driver/src/hc32_ll_cmp.c
    hc32_ll_driver/src/hc32_ll_crc.c
    hc32_ll_driver/src/hc32_ll_dbgc.c
    hc32_ll_driver/src/hc32_ll_dcu.c
    hc32_ll_driver/src/hc32_ll_dma.c
    hc32_ll_driver/src/hc32_ll_efm.c
    hc32_ll_driver/src/hc32_ll_emb.c
    hc32_ll_driver/src/hc32_ll_event_port.c
    hc32_ll_driver/src/hc32_ll_fcg.c
    hc32_ll_driver/src/hc32_ll_fcm.c
    hc32_ll_driver/src/hc32_ll_gpio.c
    hc32_ll_driver/src/hc32_ll_hash.c
    hc32_ll_driver/src/hc32_ll_i2c.c
    hc32_ll_driver/src/hc32_ll_i2s.c
    hc32_ll_driver/src/hc32_ll_icg.c
    hc32_ll_driver/src/hc32_ll_interrupts.c
    hc32_ll_driver/src/hc32_ll_keyscan.c
    hc32_ll_driver/src/hc32_ll_mpu.c
    hc32_ll_driver/src/hc32_ll_ots.c
    hc32_ll_driver/src/hc32_ll_pwc.c
    hc32_ll_driver/src/hc32_ll_qspi.c
    hc32_ll_driver/src/hc32_ll_rmu.c
    hc32_ll_driver/src/hc32_ll_rtc.c
    hc32_ll_driver/src/hc32_ll_sdioc.c
    hc32_ll_driver/src/hc32_ll_spi.c
    hc32_ll_driver/src/hc32_ll_sram.c
    hc32_ll_driver/src/hc32_ll_swdt.c
    hc32_ll_driver/src/hc32_ll_tmr0.c
    hc32_ll_driver/src/hc32_ll_tmr4.c
    hc32_ll_driver/src/hc32_ll_tmr6.c
    hc32_ll_driver/src/hc32_ll_tmra.c
    hc32_ll_driver/src/hc32_ll_trng.c
    hc32_ll_driver/src/hc32_ll_usart.c
    hc32_ll_driver/src/hc32_ll_usb.c
    hc32_ll_driver/src/hc32_ll_utility.c
    hc32_ll_driver/src/hc32_ll_wdt.c
    hc32_ll_driver/src/hc32_ll.c
    hc32_ll_driver/src/hc32f460_ll_interrupts_share.c
)

# 2. CMSIS源文件
#----------------------------------------
set(CMSIS_SOURCES
    cmsis/Device/HDSC/hc32f4xx/Source/system_hc32f460.c
)

# 3. BSP板级支持包源文件
#----------------------------------------
# 3.1 评估板相关源文件
set(BSP_BOARD_SOURCES
    bsp/ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2_24cxx.c
    bsp/ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2_w25qxx.c
    bsp/ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2_wm8731.c
    bsp/ev_hc32f460_lqfp100_v2/ev_hc32f460_lqfp100_v2.c
)

# 3.2 组件源文件
set(BSP_COMPONENT_SOURCES
    bsp/components/24cxx/24cxx.c
    bsp/components/malloc/malloc.c
    bsp/components/malloc/memory_management.cpp
    bsp/components/w25qxx/w25qxx.c
    bsp/components/wm8731/wm8731.c
)

# 3.3 外设驱动源文件
set(BSP_PERIPHERAL_SOURCES
    bsp/cpu_utilization/bsp_cpu.c
    bsp/usart/printf_usart.c
    bsp/usart/bsp_uart.cpp
    bsp/pwm/bsp_pwm.cpp
    bsp/can/bsp_can.cpp
)

# 4. 应用层源文件
#----------------------------------------
set(APP_SOURCES
    drv/drv_hal.cpp
    hal/hal.cpp
    app/taskManage.cpp
)

# 5. 合并所有源文件
#----------------------------------------
set(DRIVER_SOURCES
    ${HC32_LL_SOURCES}
    ${CMSIS_SOURCES}
    ${BSP_BOARD_SOURCES}
    ${BSP_COMPONENT_SOURCES}
    ${BSP_PERIPHERAL_SOURCES}
    ${APP_SOURCES}
)

# 6. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${DRIVER_SOURCES})

# 7. 包含目录设置
#----------------------------------------
# 7.1 CMSIS头文件路径
set(CMSIS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/Device/HDSC/hc32f4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/Include
)

# 7.2 HC32 LL驱动头文件路径
set(HC32_LL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/hc32_ll_driver/inc
)

# 7.3 BSP头文件路径
set(BSP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/ev_hc32f460_lqfp100_v2
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/components/24cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/components/w25qxx
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/components/wm8731
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/components/malloc
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/can
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/pwm
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/usart
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/cpu_utilization
)

# 7.4 应用层头文件路径
set(APP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/drv
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${CMAKE_CURRENT_SOURCE_DIR}/hal
)

# 7.5 添加所有包含路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMSIS_INCLUDE_DIRS}
    ${HC32_LL_INCLUDE_DIRS}
    ${BSP_INCLUDE_DIRS}
    ${APP_INCLUDE_DIRS}
)
