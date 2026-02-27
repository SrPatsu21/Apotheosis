#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 1, binding = 0) uniform sampler2D textures[];

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) flat in uvec4 fragMaterialIndices;

layout(location = 0) out vec4 outColor;

void main()
{
    uint baseColorIndex = fragMaterialIndices.x;

    vec4 albedo = texture(
        textures[nonuniformEXT(baseColorIndex)],
        fragTexCoord
    );

    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float NdotL = max(dot(normalize(fragNormal), lightDir), 0.0);

    outColor = vec4(albedo.rgb * NdotL, albedo.a);
}