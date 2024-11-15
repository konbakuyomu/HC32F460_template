# 1. FreeRTOS源文件
#----------------------------------------
set(FREERTOS_CORE_SOURCES
    freeRTOS/src/tasks.c
    freeRTOS/src/queue.c
    freeRTOS/src/list.c
    freeRTOS/src/event_groups.c
    freeRTOS/src/timers.c
    freeRTOS/src/stream_buffer.c
    freeRTOS/src/croutine.c
)

set(FREERTOS_PORT_SOURCES
    freeRTOS/port/MemMang/heap_4.c
    freeRTOS/port/GCC/ARM_CM4F/port.c
)

# 3. 合并所有中间件源文件
#----------------------------------------
set(MIDDLEWARE_SOURCES
    ${FREERTOS_CORE_SOURCES}
    ${FREERTOS_PORT_SOURCES}
)

# 4. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${MIDDLEWARE_SOURCES})

# 5. 添加包含路径
#----------------------------------------
# FreeRTOS头文件路径
set(FREERTOS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/port/GCC/ARM_CM4F
)

# 添加所有包含路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${FREERTOS_INCLUDE_DIRS}
)