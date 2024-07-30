#version 450

layout(location = 0) in vec2 a_FragUV;

layout(location = 0) out vec4 a_PixelColour;

layout(binding = 0) uniform sampler2D u_ImageSampler;


void main() {
    a_PixelColour = texture(u_ImageSampler,a_FragUV);
}