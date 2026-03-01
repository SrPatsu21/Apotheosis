#version 450

layout(set = 1, binding = 0) uniform sampler2D albedoTex;
layout(set = 1, binding = 1) uniform sampler2D normalTex;
layout(set = 1, binding = 2) uniform sampler2D metallicRoughnessTex;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 albedo = texture(albedoTex, fragTexCoord);

    // Normal map (tangent space simplificado — assumindo que você ainda não montou TBN)
    vec3 normal = normalize(fragNormal);

    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float NdotL = max(dot(normal, lightDir), 0.0);

    vec3 lighting = albedo.rgb * NdotL;

    outColor = vec4(lighting, albedo.a);
}