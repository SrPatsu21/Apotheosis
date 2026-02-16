#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

// UBO global
layout(std140, set = 0, binding = 0) uniform UniformBufferGlobal {
    mat4 view;
    mat4 proj;
} ubo;

layout(std430, set = 2, binding = 0) readonly buffer InstanceBuffer {
    mat4 models[];
} instanceData;

void main() {
    mat4 model = instanceData.models[gl_InstanceIndex];
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
