#version 330 core
//From Abstract instanced Tile VAO
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

//From renderable Tiles VBO
layout (location = 2) in vec2 aOffset;


uniform mat4 uTransformMatrix;
uniform mat4 uCameraMatrix;

out vec3 fColor;

void main() {
    fColor = aColor;
    //gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    //gl_Position = uTransformMatrix * vec4(aPos + aOffset, 0.0, 1.0);
    gl_Position = uTransformMatrix * uCameraMatrix * vec4(aPos + aOffset, 0.0, 1.0);
}
