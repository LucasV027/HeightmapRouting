#version 460 core

out vec4 FragColor;

in float vH;

void main() {
    FragColor = vec4(1.0, vH, 0.0, 1.0);
}
