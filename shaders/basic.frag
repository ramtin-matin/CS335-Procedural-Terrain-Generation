#version 330 core

in float vHeight;

uniform bool uUseHeightColoring;
uniform float uMinHeight;
uniform float uMaxHeight;
uniform vec3 uBaseColor;

out vec4 FragColor;

void main() {
    if (!uUseHeightColoring) {
        FragColor = vec4(uBaseColor, 1.0);
        return;
    }

    float heightRange = max(uMaxHeight - uMinHeight, 0.0001);
    float heightT = clamp((vHeight - uMinHeight) / heightRange, 0.0, 1.0);

    vec3 lowColor = vec3(0.16, 0.42, 0.16);
    vec3 midColor = vec3(0.58, 0.63, 0.32);
    vec3 highColor = vec3(0.90, 0.90, 0.90);

    vec3 terrainColor = mix(lowColor, midColor, smoothstep(0.0, 0.70, heightT));
    terrainColor = mix(terrainColor, highColor, smoothstep(0.70, 1.0, heightT));

    FragColor = vec4(terrainColor, 1.0);
}
