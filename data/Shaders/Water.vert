#version 460 core

layout (location = 0) in vec2 inPos;

uniform mat4 uVP;
uniform float uHeight;

void main() {
    gl_Position = uVP * vec4(inPos.x, uHeight, inPos.y, 1.0);
}