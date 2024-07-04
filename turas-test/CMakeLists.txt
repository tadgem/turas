cmake_minimum_required(VERSION 3.14)
project(turas-test)

add_executable(${PROJECT_NAME} test.cpp
        test.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC ${TURAS_INCLUDES})
target_link_libraries (${PROJECT_NAME} turas)
