#version 330 core
out vec4 FragColor;

//in vec3 fColor;
in vec2 fTexCoord;


uniform sampler2D uTexture[1];

void main() {
    //FragColor = vec4(fColor, 1.0);
    //FragColor = vec4(fTexCoord, 0.0, 1.0);
    FragColor = texture(uTexture[0], fTexCoord);
}
