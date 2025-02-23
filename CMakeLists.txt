cmake_minimum_required(VERSION 3.30)
project(liscpp)

set(CMAKE_CXX_STANDARD 20)
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib
)
link_directories(${PROJECT_SOURCE_DIR}/lib)
file(GLOB LIB_SRC
    "${PROJECT_SOURCE_DIR}/lib/*.cpp"
)

add_executable(liscpp main.cpp ${LIB_SRC})
