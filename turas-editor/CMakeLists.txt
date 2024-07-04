cmake_minimum_required(VERSION 3.14)
project(turas-editor)

add_executable(${PROJECT_NAME} App.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC ${LVK_INCLUDES})
target_link_libraries (${PROJECT_NAME} lvk turas)
