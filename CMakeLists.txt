cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME histogram)

project(${PROJECT_NAME})

set(SOURCE_FILES histogram.cpp)
set(HEADER_FILES histogram.hpp)

add_executable(${PROJECT_NAME}  ${HEADER_FILES} ${SOURCE_FILES} main.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/STACK:10000000")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wl,--stack,10000000, -O3)
endif()

find_program(CLANG_FORMAT "clang-format")
if (CLANG_FORMAT)
    message("CLANG-FORMAT found at: " ${CLANG_FORMAT})
    unset(SOURCE_FILES_PATHS)
    foreach(SOURCE_FILE ${SOURCE_FILES} ${HEADER_FILES})
        get_source_file_property(WHERE ${SOURCE_FILE} LOCATION)
        set(SOURCE_FILES_PATHS ${SOURCE_FILES_PATHS} ${WHERE})
    endforeach()
    add_custom_target(
        ClangFormat
        COMMAND ${CLANG_FORMAT}
        -i
        -style=file
        ${SOURCE_FILES_PATHS})
    add_dependencies(${PROJECT_NAME} ClangFormat)
else()
    message("Unable to find Clang-Format")
endif()