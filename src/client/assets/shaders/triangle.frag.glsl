#version 450

layout(set = 1, binding = 0) uniform sampler2D textures[];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in uvec4 fragMaterialIndices;

layout(location = 0) out vec4 outColor;

void main()
{
    uint baseColorIndex = fragMaterialIndices.x;

    vec4 texColor = texture(textures[baseColorIndex], fragTexCoord);

    outColor = texColor * fragColor;
}