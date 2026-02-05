#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uVP;
uniform mat4 uModel;

out vec3 vNormal;

void main() {
    vNormal = normalize(aNormal);
    gl_Position = uVP * uModel * vec4(aPos, 1.0);
}

