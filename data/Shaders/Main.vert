#version 460 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;

uniform mat4 uVP;
uniform float uHeightScale;

layout (binding = 0) uniform sampler2D uHeightMap;
layout (binding = 1) uniform sampler2D uNormalMap;

out float vH;
out vec3 vNormal;

void main() {
    float h = texture(uHeightMap, inUV).r;
    vec3 worldPos = vec3(inPos.x, h * uHeightScale, inPos.y);

    vH = h;
    vNormal = texture(uNormalMap, inUV).rgb;

    gl_Position = uVP * vec4(worldPos, 1.0);
}