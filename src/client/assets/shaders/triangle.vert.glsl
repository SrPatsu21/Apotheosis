#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uvec4 fragMaterialIndices;

layout(std140, set = 0, binding = 0) uniform UniformBufferGlobal {
    mat4 view;
    mat4 proj;
} ubo;

struct InstanceData {
    mat4 model;
    uvec4 materialIndices;
};

layout(std430, set = 2, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceBuffer;

void main()
{
    InstanceData instance = instanceBuffer.instances[gl_InstanceIndex];

    gl_Position = ubo.proj * ubo.view * instance.model * vec4(inPosition, 1.0);

    fragColor = inColor;
    fragTexCoord = inTexCoord;

    fragMaterialIndices = instance.materialIndices;
}