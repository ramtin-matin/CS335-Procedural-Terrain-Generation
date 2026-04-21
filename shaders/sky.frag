#version 330 core

in vec2 vScreenUv;

uniform vec3 uSkyBottomColor;
uniform vec3 uSkyTopColor;

out vec4 FragColor;

void main() {
    float skyT = smoothstep(0.0, 1.0, clamp(vScreenUv.y, 0.0, 1.0));
    vec3 skyColor = mix(uSkyBottomColor, uSkyTopColor, skyT);
    FragColor = vec4(skyColor, 1.0);
}
