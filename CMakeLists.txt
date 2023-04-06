cmake_minimum_required(VERSION 3.7)

set(CMAKE_BUILD_TYPE Debug)
set(SHORT_NAME cmonkey)
project(${SHORT_NAME} C)
include_directories(include src)

set(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/include/cwalk/cwalk.c
    ${PROJECT_SOURCE_DIR}/include/array_list/array_list.c
    ${PROJECT_SOURCE_DIR}/src/lexer/lexer.c
)

add_executable(${SHORT_NAME} ${SOURCE_FILES} ${PROJECT_SOURCE_DIR}/src/main/main.c)
add_executable(ctests ${SOURCE_FILES} ${PROJECT_SOURCE_DIR}/src/tests/tests.c)

target_link_libraries(${SHORT_NAME} m)
target_link_libraries(${SHORT_NAME} readline)

find_package (Threads REQUIRED)
target_link_libraries(${SHORT_NAME} ${CMAKE_THREAD_LIBS_INIT})
