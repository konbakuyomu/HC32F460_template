# 1. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_hal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal.cpp
)

# 2. 添加头文件目录
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

