#version 450

layout(location = 0) in vec3 a_VertexPosition;
layout(location = 1) in vec2 a_VertexUV;

layout(location = 0) out vec2 a_FragUV;

void main() {
    // I have made a terrible mistake, yet again :(
    gl_Position = vec4(a_VertexPosition, 1.0);
    a_FragUV = a_VertexUV;
}