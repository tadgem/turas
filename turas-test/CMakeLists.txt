cmake_minimum_required(VERSION 3.14)
project(turas-test)
add_executable(${PROJECT_NAME} test.cpp
        test.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC ${TURAS_INCLUDES})
target_link_libraries (${PROJECT_NAME} turas)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Sponza DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/crate.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_shaders_dependency(turas-test)