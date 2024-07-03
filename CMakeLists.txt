cmake_minimum_required(VERSION 3.14)
project(turas-all)

# dependency / cmake options
set(SDL_STATIC TRUE CACHE INTERNAL TRUE)
set(CMAKE_SUPPRESS_REGENERATION true)
set(SDL2_DISABLE_SDL2MAIN       ON      CACHE BOOL "Disable SDL Entrypoint")
set(SDL2_DISABLE_INSTALL        ON      CACHE BOOL "Disable SDL Install")
set(SDL2_DISABLE_UNINSTALL      ON      CACHE BOOL "Disable SDL Uninstall")
set(BUILD_SHARED_LIBS           OFF     CACHE BOOL "Disable ASSIMP dynamic linking")
set(SKIP_PERFORMANCE_COMPARISON TRUE CACHE BOOL "Disable perf. comparison which requires boost *shudder*")


add_subdirectory(submodules/cereal)
add_subdirectory(submodules/entt)
add_subdirectory(submodules/lvk)

set(CEREAL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/submodules/cereal/include)
set(ENTT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/submodules/entt/src)

add_subdirectory(turas)
add_subdirectory(turas-editor)
