#version 330 core
//From Abstract instanced Tile VAO
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

//From renderable Tiles VBO
layout (location = 2) in vec2 aOffset;
layout (location = 3) in vec2 aTexOffset;


uniform mat4 uTransformMatrix;
uniform mat4 uCameraMatrix;

//out vec3 fColor;
out vec2 fTexCoord;

void main() {
    fTexCoord = aTexCoord+aTexOffset;
    //fTexCoord = aTexCoord;
    gl_Position = uTransformMatrix * uCameraMatrix * vec4(aPos + aOffset, 0.0, 1.0);
}
