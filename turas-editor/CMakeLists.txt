cmake_minimum_required(VERSION 3.14)
project(turas-editor)

add_executable(${PROJECT_NAME} App.cpp EditorEngine.cpp EditorEngine.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC ${TURAS_INCLUDES})
target_link_libraries(${PROJECT_NAME} turas)

file(COPY ${TURAS_SHADERS_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_shaders_dependency(turas-editor)