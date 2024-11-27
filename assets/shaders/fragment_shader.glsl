// fragment_shader.glsl

#version 330 core
in vec2 TexCoords;
flat in int BlockType; // Recebe o tipo de bloco

uniform sampler2D grassTexture;
uniform sampler2D dirtTexture;
uniform sampler2D stoneTexture;

out vec4 FragColor;

void main() {
    vec4 color;
    if (BlockType == 1) {
        color = texture(grassTexture, TexCoords);
    } else if (BlockType == 2) {
        color = texture(dirtTexture, TexCoords);
    } else if (BlockType == 3) {
        color = texture(stoneTexture, TexCoords);
    } else {
        color = vec4(1.0); // Branco para blocos desconhecidos
    }
    FragColor = color;
}
