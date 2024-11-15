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

# 2. 驱动层源文件
#----------------------------------------
set(HARDWARE_SOURCES
    hardware/can/drvCan.cpp
    hardware/motor/drvMotor.cpp
    hardware/uart/drvUart.cpp
)

# 3. 合并所有源文件
#----------------------------------------
set(DRIVER_SOURCES
    ${HC32_LL_SOURCES}
    ${HARDWARE_SOURCES}
)

# 4. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${DRIVER_SOURCES})

# 5. 包含目录设置
#----------------------------------------
# 5.1 CMSIS头文件路径
set(CMSIS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/Device/HDSC/hc32f4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/Include
)

# 5.2 HC32 LL驱动头文件路径
set(HC32_LL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/hc32_ll_driver/inc
)

# 5.3 驱动层头文件路径
set(HARDWARE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware/can
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware/motor
    ${CMAKE_CURRENT_SOURCE_DIR}/hardware/uart
)

# 6. 添加所有包含路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMSIS_INCLUDE_DIRS}
    ${HC32_LL_INCLUDE_DIRS}
    ${HARDWARE_INCLUDE_DIRS}
)
