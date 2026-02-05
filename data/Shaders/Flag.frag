#version 460 core

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightDir = vec3(0.0, 1.0, 0.3);

in vec3 vNormal;

void main() {
    vec3 lightDir = normalize(uLightDir);
    vec3 normal = normalize(vNormal);
    float diffuse = max(dot(normal, lightDir), 0.0);

    float ambient = 0.3;
    vec3 litColor = uColor * (ambient + diffuse * 0.7);

    FragColor = vec4(litColor, 1.0);
}