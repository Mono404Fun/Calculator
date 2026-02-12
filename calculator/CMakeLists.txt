cmake_minimum_required(VERSION 3.16)

project(calculator VERSION 1.0.0 LANGUAGES CXX)

# setting C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# adding compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_definitions(-DFMT_HEADER_ONLY)
endif()


# include directory (headers)
include_directories(${PROJECT_SOURCE_DIR}/include)

# source files
set(SOURCES
    # src/expression.cpp
    src/main.cpp
    src/token.cpp
    src/expression.cpp
    src/operator.cpp
    src/logic.cpp
)

# create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Precompiled headers for faster builds
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include/pch.hpp)
target_include_directories(calculator PRIVATE /usr/include)  # usually where fmt/core.h is

# set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# optimization flags
if(MSVC)
    # compilation optimizations: O2 and whole program optimization in release
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/O2 /GL>
    )
    # linking optimizations: link-time code generation and code folding in release
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
    )
else()
    # compilation optimizations: LTO and dead code elimination in release
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3 -Os -flto -ffunction-sections -fdata-sections>
    )
    # linking optimizations
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-flto -Wl,--gc-sections>
    )
endif()

