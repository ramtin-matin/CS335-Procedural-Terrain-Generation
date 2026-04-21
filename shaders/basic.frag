#version 330 core

in vec3 vWorldPosition;
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

    vec3 lowColor = vec3(0.11, 0.33, 0.13);
    vec3 midColor = vec3(0.42, 0.55, 0.21);
    vec3 highColor = vec3(0.61, 0.54, 0.41);
    vec3 peakColor = vec3(0.88, 0.89, 0.91);

    vec3 terrainColor = mix(lowColor, midColor, smoothstep(0.00, 0.42, heightT));
    terrainColor = mix(terrainColor, highColor, smoothstep(0.48, 0.80, heightT));
    terrainColor = mix(terrainColor, peakColor, smoothstep(0.86, 1.00, heightT));

    vec3 dpdx = dFdx(vWorldPosition);
    vec3 dpdy = dFdy(vWorldPosition);
    vec3 surfaceNormal = normalize(cross(dpdx, dpdy));
    if (!gl_FrontFacing) {
        surfaceNormal *= -1.0;
    }

    vec3 lightDirection = normalize(vec3(-0.45, 0.85, 0.30));
    float diffuse = max(dot(surfaceNormal, lightDirection), 0.0);
    float lighting = 0.45 + diffuse * 0.55;

    FragColor = vec4(terrainColor * lighting, 1.0);
}
