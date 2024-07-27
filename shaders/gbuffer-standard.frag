#version 450

layout(location = 0) in vec3 a_FragPosition;
layout(location = 1) in vec3 a_FragNormal;
layout(location = 2) in vec2 a_FragUV;

layout(location = 0) out vec4 a_PixelPosition;
layout(location = 1) out vec4 a_PixelNormal;
layout(location = 2) out vec4 a_PixelColour;

layout(binding = 1) uniform sampler2D u_DiffuseSampler;

void main() {
    a_PixelColour = vec4(texture(u_DiffuseSampler, a_FragUV).rgb, 1.0);
    a_PixelPosition = vec4(a_FragPosition, 1.0);
    a_PixelNormal = vec4(a_FragNormal, 1.0);
}