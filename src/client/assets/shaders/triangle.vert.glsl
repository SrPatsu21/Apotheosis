#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragNormal;
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

    mat4 model = instance.model;
    mat3 normalMatrix = mat3(transpose(inverse(model)));

    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 1.0);

    fragTexCoord = inTexCoord;
    fragNormal = normalize(normalMatrix * inNormal);
    fragMaterialIndices = instance.materialIndices;
}