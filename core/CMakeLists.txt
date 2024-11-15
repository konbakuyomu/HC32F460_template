# 1. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/system/systemHc32f460.c
    ${CMAKE_CURRENT_SOURCE_DIR}/system/systemCalls.c
)