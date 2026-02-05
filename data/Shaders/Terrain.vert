#version 460 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

layout(binding = 0) uniform sampler2D uHeightMap;
layout(binding = 1) uniform sampler2D uNormalMap;
layout(binding = 2) uniform usampler2D uTypeMap;

uniform mat4 uVP;
uniform float uHeightScale;

out VS_OUT {
    float height;
    vec3 normal;
    vec3 worldPos;
    flat uint type;
} vs_out;

void main() {
    float height = texture(uHeightMap, aTexCoord).r;
    vec3 normal = texture(uNormalMap, aTexCoord).rgb;
    uint type = texture(uTypeMap, aTexCoord).r;

    vec3 worldPos = vec3(aPosition.x, height * uHeightScale, aPosition.y);

    vs_out.height = height;
    vs_out.normal = normal;
    vs_out.worldPos = worldPos;
    vs_out.type = type;

    gl_Position = uVP * vec4(worldPos, 1.0);
}