#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 uTransformMatrix;
uniform mat4 uCameraMatrix;

out vec3 fColor;

void main() {
    fColor = aColor;
    gl_Position = uTransformMatrix * uCameraMatrix * vec4(aPos, 0.0, 1.0);
}
