#version 450

layout(location = 0) in vec3 a_VertexPosition;
layout(location = 1) in vec3 a_VertexNormal;
layout(location = 2) in vec2 a_VertexUV;

layout(location = 0) out vec3 a_FragPosition;
layout(location = 1) out vec3 a_FragNormal;
layout(location = 2) out vec2 a_FragUV;

layout(push_constant, std430) uniform PUSH_CONSTANTS {
    mat4 pc_Model;
    mat4 pc_View;
    mat4 pc_Proj;
};

void main() {

    a_FragUV = a_VertexUV;

    mat4 normalMatrix = transpose(inverse(pc_Model));
    a_FragNormal = normalize((normalMatrix * vec4(a_VertexNormal, 0)).xyz);

    a_FragPosition = (model * vec4(a_VertexPosition, 1.0)).xyz;
    gl_Position = pc_Proj * pc_View * vec4(Position, 1.0);
}