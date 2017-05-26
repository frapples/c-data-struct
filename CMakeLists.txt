cmake_minimum_required(VERSION 3.4)

project(data-struct)


message(STATUS "工程目录位于： ${PROJECT_BINARY_DIR}")
message(STATUS "构建目录位于： " ${PROJECT_SOURCE_DIR})


set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb -std=c99")
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall -std=c99")

include_directories("${PROJECT_SOURCE_DIR}/lib")

# cmockery
include_directories("${PROJECT_SOURCE_DIR}/../cmockery")
link_directories("${PROJECT_SOURCE_DIR}/../cmockery")


aux_source_directory("lib/" LIB_SRCS)
add_library("data-struct" STATIC ${LIB_SRCS})


aux_source_directory("test/" TEST_SRCS)
add_executable(unit_test ${TEST_SRCS})
target_link_libraries(unit_test "data-struct" "cmockery")
