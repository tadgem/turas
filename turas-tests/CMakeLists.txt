cmake_minimum_required(VERSION 3.14)
project(turas-tests)

add_executable(turas-engine-tests test.cpp test.h)
set_property(TARGET turas-engine-tests PROPERTY CXX_STANDARD 17)
target_include_directories(turas-engine-tests PUBLIC ${TURAS_INCLUDES})
target_link_libraries(turas-engine-tests turas)

add_executable(turas-imgui-style-test imgui_style_test.cpp)
set_property(TARGET turas-imgui-style-test PROPERTY CXX_STANDARD 17)
target_include_directories(turas-imgui-style-test PUBLIC ${TURAS_INCLUDES})
target_link_libraries(turas-imgui-style-test turas)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Sponza DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/crate.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_shaders_dependency(turas-engine-tests)
add_shaders_dependency(turas-imgui-style-test)