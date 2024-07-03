cmake_minimum_required(VERSION 3.14)
project(turas-editor)

add_executable(${PROJECT_NAME} App.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${LVK_INCLUDES})
target_link_libraries (${PROJECT_NAME} lvk turas)
