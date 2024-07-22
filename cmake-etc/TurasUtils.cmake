find_package(Vulkan REQUIRED)

function(add_shaders DEPENDENCY_TARGET SHADERS_DIR)
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
    file(GLOB_RECURSE GLSL_SOURCE_FILES
            "${SHADERS_DIR}/*.frag"
            "${SHADERS_DIR}/*.vert"
            "${SHADERS_DIR}/*.comp"
    )

    foreach (GLSL ${GLSL_SOURCE_FILES})
        get_filename_component(FILE_NAME ${GLSL} NAME)
        set(SPIRV "${SHADERS_DIR}/${FILE_NAME}.spv")
        add_custom_command(
                OUTPUT ${SPIRV}
                COMMENT "Compile Shader: ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}"
                COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
                DEPENDS ${GLSL})
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
    endforeach (GLSL)

    add_custom_target(
            Shaders
            DEPENDS ${SPIRV_BINARY_FILES}
    )

    add_dependencies(${DEPENDENCY_TARGET} Shaders)

    set(INTERNAL_SHADERS_DIR_HACK ${SHADERS_DIR} CACHE INTERNAL "")
endfunction()

function(add_shaders_dependency DEPENDENCY_TARGET)
    add_dependencies(${DEPENDENCY_TARGET} Shaders)

    add_custom_command(TARGET ${DEPENDENCY_TARGET}
            DEPENDS Shaders
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${INTERNAL_SHADERS_DIR_HACK}"
            "${CMAKE_CURRENT_BINARY_DIR}/shaders/")
endfunction()