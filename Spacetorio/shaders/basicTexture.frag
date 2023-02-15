#version 330 core
out vec4 FragColor;

in vec2 fTexCoord;

uniform sampler2D uTexture[1];

void main() {
    FragColor = texture(uTexture[0], fTexCoord);
}
