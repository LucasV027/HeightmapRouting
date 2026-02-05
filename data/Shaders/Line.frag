#version 460 core

out vec4 FragColor;

uniform vec3 uColor = vec3(0.5);

void main() {
    FragColor = vec4(uColor, 1.0);
}