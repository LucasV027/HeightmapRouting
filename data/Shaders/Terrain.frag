#version 460 core

out vec4 FragColor;

in float vH;
in vec3 vNormal;

uniform vec3 uLightDir = vec3(-0.5, 1.0, -0.3);
uniform vec3 uAmbient = vec3(0.3);
uniform vec3 uLightColor = vec3(1.0);

struct HeightColor {
    float height;
    vec3 color;
};

HeightColor HC[5] = HeightColor[](
HeightColor(0.00, vec3(0.2, 0.6, 0.2)), // Green
HeightColor(0.25, vec3(0.8, 0.7, 0.3)), // Yellow
HeightColor(0.50, vec3(0.7, 0.5, 0.3)), // Brown
HeightColor(0.75, vec3(0.6, 0.6, 0.6)), // Gray
HeightColor(1.00, vec3(1.0, 1.0, 1.0))  // White
);

vec3 ColorFromHeight(float height) {
    for (int i = 0; i < HC.length() - 1; i++) {
        if (height < HC[i + 1].height) {
            float range = HC[i + 1].height - HC[i].height;
            float t = (height - HC[i].height) / range;
            return mix(HC[i].color, HC[i + 1].color, t);
        }
    }
    return HC[HC.length() - 1].color;
}

void main() {
    vec3 baseColor = ColorFromHeight(vH);
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightDir);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 lighting = uAmbient + diffuse;
    vec3 finalColor = baseColor * lighting;

    FragColor = vec4(finalColor, 1.0);
}