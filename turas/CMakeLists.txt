cmake_minimum_required(VERSION 3.14)
project(turas)

file(GLOB_RECURSE CORE_SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" src/*.cpp)
file(GLOB_RECURSE CORE_HEADER_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.h)
file(GLOB_RECURSE CORE_HEADERPP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" include/*.hpp)

foreach(header IN LISTS CORE_HEADER_FILES)
    get_filename_component(source_path "${header}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${header}")
endforeach()

foreach(header IN LISTS CORE_HEADERPP_FILES)
    get_filename_component(source_path "${header}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${header}")
endforeach()

foreach(source IN LISTS CORE_SRC_FILES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

add_library(${PROJECT_NAME} STATIC ${CORE_SRC_FILES} ${CORE_HEADER_FILES} ${CORE_HEADERPP_FILES}
        include/Rendering/VertexLayouts.h)

set(TURAS_INCLUDES ${LVK_INCLUDES} ${CEREAL_INCLUDES} ${ENTT_INCLUDES} ${ASSIMP_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

target_include_directories (${PROJECT_NAME} PUBLIC ${TURAS_INCLUDES})

if (MSVC)
    # compile options (bigobjs, compile on multiple cores, Zi allows release builds to be debugged, /X removes STL
    target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj" "/MP" "/Zi")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,Release>:ProgramDatabase>")
endif ()

target_link_libraries (${PROJECT_NAME} lvk cereal assimp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)