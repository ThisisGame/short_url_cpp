cmake_minimum_required(VERSION 3.10)
project(short_url_cpp)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    add_compile_options(/wd4819)#忽略保存为unicode警告
endif ()

#头文件目录
include_directories("depends")
include_directories("source")
include_directories("depends/stduuid/gsl")

file(GLOB_RECURSE fmt_cpp "depends/fmt/*.cpp")
file(GLOB_RECURSE engine_cpp "source/*.cpp")

add_executable(short_url_cpp ${engine_cpp})