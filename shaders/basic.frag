#version 330 core

in vec3 vWorldPosition;
in float vHeight;
in float vViewDistance;

uniform bool uUseHeightColoring;
uniform float uMinHeight;
uniform float uMaxHeight;
uniform vec3 uBaseColor;
uniform vec3 uCameraPosition;
uniform vec3 uSkyBottomColor;
uniform vec3 uSkyTopColor;
uniform float uFogNear;
uniform float uFogFar;

out vec4 FragColor;

void main() {
    if (!uUseHeightColoring) {
        FragColor = vec4(uBaseColor, 1.0);
        return;
    }

    float heightRange = max(uMaxHeight - uMinHeight, 0.0001);
    float heightT = clamp((vHeight - uMinHeight) / heightRange, 0.0, 1.0);

    vec3 lowColor = vec3(0.17, 0.21, 0.16);
    vec3 midColor = vec3(0.36, 0.48, 0.24);
    vec3 highColor = vec3(0.58, 0.52, 0.40);
    vec3 peakColor = vec3(0.84, 0.86, 0.88);

    vec3 terrainColor = mix(lowColor, midColor, smoothstep(0.08, 0.42, heightT));
    terrainColor = mix(terrainColor, highColor, smoothstep(0.48, 0.78, heightT));
    terrainColor = mix(terrainColor, peakColor, smoothstep(0.84, 1.00, heightT));

    vec3 dpdx = dFdx(vWorldPosition);
    vec3 dpdy = dFdy(vWorldPosition);
    vec3 surfaceNormal = normalize(cross(dpdx, dpdy));
    if (!gl_FrontFacing) {
        surfaceNormal *= -1.0;
    }

    vec3 lightDirection = normalize(vec3(-0.38, 0.88, 0.27));
    vec3 viewDirection = normalize(uCameraPosition - vWorldPosition);
    vec3 halfVector = normalize(lightDirection + viewDirection);

    float diffuse = max(dot(surfaceNormal, lightDirection), 0.0);
    float softenedDiffuse = 0.35 + diffuse * 0.65;
    float specular = pow(max(dot(surfaceNormal, halfVector), 0.0), 20.0) * 0.10;
    float upLight = clamp(surfaceNormal.y * 0.5 + 0.5, 0.0, 1.0);
    float lighting = 0.24 + softenedDiffuse * 0.60 + upLight * 0.16 + specular;

    float fogAmount = smoothstep(uFogNear, uFogFar, vViewDistance);
    float fogHeightT = clamp((vWorldPosition.y - uMinHeight) / max(heightRange * 1.4, 0.0001), 0.0, 1.0);
    vec3 fogColor = mix(uSkyBottomColor, uSkyTopColor, 0.35 + fogHeightT * 0.45);

    vec3 litColor = terrainColor * lighting;
    FragColor = vec4(mix(litColor, fogColor, fogAmount * 0.32), 1.0);
}
