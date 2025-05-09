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
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zi")
else()
    if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    endif()
endif()

add_executable(liscpp main.cpp ${LIB_SRC})
