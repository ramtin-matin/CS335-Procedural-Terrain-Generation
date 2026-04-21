#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out float vHeight;

void main() {
    vHeight = aPosition.y;
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
