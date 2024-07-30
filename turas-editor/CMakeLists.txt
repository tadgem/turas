cmake_minimum_required(VERSION 3.14)
project(turas-editor)

file(GLOB_RECURSE EDITOR_SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)
file(GLOB_RECURSE EDITOR_HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
file(GLOB_RECURSE EDITOR_HEADERPP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.hpp)

foreach (header IN LISTS EDITOR_HEADER_FILES)
    get_filename_component(source_path "${header}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${header}")
endforeach ()

foreach (header IN LISTS EDITOR_HEADERPP_FILES)
    get_filename_component(source_path "${header}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${header}")
endforeach ()

foreach (source IN LISTS EDITOR_SRC_FILES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach ()

add_executable(${PROJECT_NAME} ${EDITOR_HEADER_FILES} ${EDITOR_HEADERPP_FILES} ${EDITOR_SRC_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC ${TURAS_INCLUDES})
target_link_libraries(${PROJECT_NAME} turas)

file(COPY ${TURAS_SHADERS_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_shaders_dependency(turas-editor)