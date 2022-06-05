cmake_minimum_required(VERSION 3.22.2)

project(trimRight LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_custom_command(
    TARGET ${PROJECT_NAME} 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/lorem.txt
            ${CMAKE_BINARY_DIR}/lorem.txt)
