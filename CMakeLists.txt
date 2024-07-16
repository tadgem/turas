cmake_minimum_required(VERSION 3.14)
project(turas-all)
include(cmake-etc/TurasUtils.cmake)

# dependency / cmake options
set(SDL_STATIC TRUE CACHE INTERNAL TRUE)
set(CMAKE_SUPPRESS_REGENERATION true)
set(SDL2_DISABLE_SDL2MAIN       ON      CACHE BOOL "Disable SDL Entrypoint")
set(SDL2_DISABLE_INSTALL        ON      CACHE BOOL "Disable SDL Install")
set(SDL2_DISABLE_UNINSTALL      ON      CACHE BOOL "Disable SDL Uninstall")
set(BUILD_SHARED_LIBS           OFF     CACHE BOOL "Disable ASSIMP dynamic linking")
set(SKIP_PERFORMANCE_COMPARISON TRUE CACHE BOOL "Disable perf. comparison which requires boost *shudder*")

file(GLOB_RECURSE TURAS_VERT_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" shaders/*.vert)
file(GLOB_RECURSE TURAS_FRAG_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" shaders/*.frag)


add_subdirectory(submodules/cereal)
add_subdirectory(submodules/entt)
add_subdirectory(submodules/lvk)
add_subdirectory(submodules/efsw)
add_subdirectory(third-party/tracy)

set(CEREAL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/submodules/cereal/include)
set(ENTT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/submodules/entt/src)
set(EFSW_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/submodules/efsw/include)
set(TRACY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/third-party/tracy/public)
set(TURAS_SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

message("-- Successfully Configured Turas Third Party Dependencies")

message("-- Configuring Turas Engine")
add_subdirectory(turas)
message("-- Configuring Turas Editor")
add_subdirectory(turas-editor)
message("-- Configuring Turas Test Suite")
add_subdirectory(turas-test)

message("-- Configuring Turas Depedencies")
add_dependencies(turas-editor turas)
add_dependencies(turas-test turas)

