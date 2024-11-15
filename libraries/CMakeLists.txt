# 1. QF库源文件
#----------------------------------------
set(QF_LIB_SOURCES
    qf_lib/src/wire.c
    qf_lib/src/bmp280.c
    qf_lib/src/appManage.c
    qf_lib/src/button.c
    qf_lib/src/keyValueMap.cpp
    qf_lib/src/keyValueTransation.c
    qf_lib/src/beatTask.c
    qf_lib/src/ringBuffer.c
)

# 2. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${QF_LIB_SOURCES})

# 3. 添加头文件目录
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/inc
)