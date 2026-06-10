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

layout(std430, set = 2, binding = 0)
readonly buffer InstanceBuffer
{
    mat4 models[];
};

layout(std430, set = 3, binding = 0)
readonly buffer BoneOffsetBuffer
{
    uint boneOffsets[];
};

layout(std430, set = 4, binding = 0)
readonly buffer BoneBuffer
{
    mat4 bones[];
};

void main()
{
    mat4 model =
        models[gl_InstanceIndex];

    uint boneOffset =
        boneOffsets[gl_InstanceIndex];

    mat4 skinMatrix =
        mat4(0.0);

    skinMatrix +=
        bones[boneOffset + inBoneIndices.x] *
        inBoneWeights.x;

    skinMatrix +=
        bones[boneOffset + inBoneIndices.y] *
        inBoneWeights.y;

    skinMatrix +=
        bones[boneOffset + inBoneIndices.z] *
        inBoneWeights.z;

    skinMatrix +=
        bones[boneOffset + inBoneIndices.w] *
        inBoneWeights.w;

    vec4 localPos =
        skinMatrix *
        vec4(
            inPosition,
            1.0
        );

    vec4 worldPos =
        model *
        localPos;

    gl_Position =
        ubo.proj *
        ubo.view *
        worldPos;

    fragWorldPos =
        worldPos.xyz;

    fragTexCoord =
        inTexCoord;

    vec3 localNormal =
        mat3(skinMatrix) *
        inNormal;

    mat3 normalMatrix =
        mat3(
            transpose(
                inverse(model)
            )
        );

    fragNormal =
        normalize(
            normalMatrix *
            localNormal
        );
}