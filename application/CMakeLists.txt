# 1. 应用层任务源文件
#----------------------------------------
set(APP_TASK_SOURCES
    can/canTask.cpp
    led/ledTask.cpp
    motor/motorTask.cpp
    uart/uartTask.cpp
)

# 2. 应用层工厂源文件
#----------------------------------------
set(APP_FACTORY_SOURCES
    taskFactory.cpp
)

# 3. 合并所有应用层源文件
#----------------------------------------
set(APPLICATION_SOURCES
    ${APP_TASK_SOURCES}
    ${APP_FACTORY_SOURCES}
)

# 4. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${APPLICATION_SOURCES})

# 5. 包含目录设置
#----------------------------------------
# 5.1 基础任务头文件路径
set(BASE_TASK_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 5.2 具体任务头文件路径
set(TASK_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/can
    ${CMAKE_CURRENT_SOURCE_DIR}/led
    ${CMAKE_CURRENT_SOURCE_DIR}/motor
    ${CMAKE_CURRENT_SOURCE_DIR}/uart
)

# 6. 添加所有包含路径
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BASE_TASK_INCLUDE_DIRS}
    ${TASK_INCLUDE_DIRS}
)