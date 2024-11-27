// vertex_shader.glsl

#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in int aBlockType;

out vec2 TexCoords;
flat out int BlockType; // Usa 'flat' para evitar interpolação

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    BlockType = aBlockType;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
