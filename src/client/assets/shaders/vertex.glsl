#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

// Uniform Buffer Object
layout(std140, set = 0, binding = 0) uniform UniformBufferObject {
    vec2 foo;          // offset 0
    vec2 _padding;     // offset 8
    mat4 model;        // offset 16
    mat4 view;         // offset 80
    mat4 proj;         // offset 144
} ubo;


void main() {
    fragColor = inColor;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}
