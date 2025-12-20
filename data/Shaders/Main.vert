#version 460 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;

uniform mat4 uVP;
uniform sampler2D uHeightMap;
uniform float uHeightScale;

out float vH;

void main()
{
    float h = texture(uHeightMap, inUV).r;

    vec3 worldPos = vec3(inPos.x, h * uHeightScale, inPos.y);

    vH = h;
    gl_Position = uVP * vec4(worldPos, 1.0);
}