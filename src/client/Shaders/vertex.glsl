#version 450

layout(location = 0) in vec2 inPosition;   // Vertex position
layout(location = 1) in vec3 inColor;      // Vertex color

layout(location = 0) out vec3 fragColor;   // Pass to fragment shader

layout(set = 0, binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    fragColor = inColor;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
}