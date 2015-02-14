cmake_minimum_required(VERSION 2.6)

project(BFInterpreter)

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
SET(GCC_COVERAGE_COMPILE_FLAGS "-O3")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

file(

     GLOB_RECURSE

     source_files

     src/*
)

add_executable(

    BFInterpreter

    ${source_files}
)