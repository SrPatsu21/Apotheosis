#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in uvec4 inBoneIndices;
layout(location = 5) in vec4 inBoneWeights;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragWorldPos;

layout(std140, set = 0, binding = 0) uniform UniformBufferGlobal
{
    mat4 view;
    mat4 proj;
} ubo;

struct InstanceData
{
    mat4 model;
};

layout(std430, set = 2, binding = 0) readonly buffer InstanceBuffer
{
    InstanceData instances[];
} instanceBuffer;

layout(std430, set = 3, binding = 0) readonly buffer BoneBuffer
{
    mat4 bones[];
};

mat4 skinMatrix =
      inBoneWeights.x * bones[inBoneIndices.x]
    + inBoneWeights.y * bones[inBoneIndices.y]
    + inBoneWeights.z * bones[inBoneIndices.z]
    + inBoneWeights.w * bones[inBoneIndices.w];

vec4 localPos =
    skinMatrix *
    vec4(inPosition, 1.0);

    mat4 model =
    instanceBuffer.instances[
        gl_InstanceIndex
    ].model;

vec4 worldPos =
    model *
    localPos;

gl_Position =
    ubo.proj *
    ubo.view *
    worldPos;

fragWorldPos = worldPos.xyz;

fragTexCoord = inTexCoord;

vec3 localNormal =
    mat3(skinMatrix) *
    inNormal;

mat3 normalMatrix =
    mat3(transpose(inverse(model)));

fragNormal =
    normalize(
        normalMatrix *
        localNormal
    );