#version 460 core

in VS_OUT {
    float height;
    vec3 normal;
    vec3 worldPos;
    flat uint type;
} fs_in;

uniform vec3 uLightDir = vec3(0.0, 1.0, 0.3);

out vec4 FragColor;

// Terrain types
const uint TYPE_NORMAL = 0u;
const uint TYPE_WATER = 1u;
const uint TYPE_FOREST = 2u;

// Contour line parameters
const float LINE_FREQUENCY = 10.0;
const float LINE_WIDTH = 0.05;
const float LINE_HEIGHT_THRESHOLD = 0.2;
const float LINE_INTENSITY = 0.5;

vec3 GetTerrainColor(float height, uint type) {
    vec3 baseColor = mix(vec3(0.3, 0.3, 0.35), vec3(0.6, 0.6, 0.65), height);

    if (type == TYPE_FOREST) baseColor = vec3(0.2, 0.36, 0.3);

    return baseColor;
}

float CalculateContourLines(float height) {
    float line = fract(height * LINE_FREQUENCY);
    line = smoothstep(0.5 - LINE_WIDTH, 0.5, line) - smoothstep(0.5, 0.5 + LINE_WIDTH, line);
    float lineFade = smoothstep(0.0, LINE_HEIGHT_THRESHOLD, height);
    return line * lineFade;
}

vec3 CalculateLighting(vec3 baseColor, vec3 normal) {
    vec3 lightDir = normalize(uLightDir);
    vec3 norm = normalize(normal);

    float diffuse = max(dot(norm, lightDir), 0.0);

    float ambient = 0.3;
    return baseColor * (ambient + diffuse * 0.7);
}

void main() {
    vec3 baseColor = GetTerrainColor(fs_in.height, fs_in.type);

    vec3 litColor = CalculateLighting(baseColor, fs_in.normal);

    float contourLine = CalculateContourLines(fs_in.height);
    vec3 lineColor = vec3(0.2, 0.2, 0.25);
    vec3 finalColor = mix(litColor, lineColor, contourLine * LINE_INTENSITY);

    FragColor = vec4(finalColor, 1.0);
}