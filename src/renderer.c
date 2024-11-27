// src/renderer.c

#include "renderer.h"

#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"
#include "camera.h"
#include "player.h"
#include "world.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Variáveis e recursos de renderização
static GLuint shaderProgram;
static GLuint VAO, VBO, EBO;
static GLuint textures[3];  // Texturas para grama, terra e pedra

// Funções auxiliares para carregar shaders e arquivos
static char* read_file(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Não foi possível abrir o arquivo %s\n", filename);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);
  char* content = (char*)malloc(length + 1);
  fread(content, 1, length, file);
  content[length] = '\0';
  fclose(file);
  return content;
}

static GLuint compile_shader(const char* source, GLenum type) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    fprintf(stderr, "Erro OpenGL (glCompileShader): %d\n", error);
  }
  // Verifica erros de compilação
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "Erro na compilação do shader: %s\n", infoLog);
  }
  return shader;
}

void renderer_init() {
  // Inicializa shaders, carrega texturas, configura buffers

  // Carrega e compila os shaders
  char* vertexShaderSource = read_file("assets/shaders/vertex_shader.glsl");
  char* fragmentShaderSource = read_file("assets/shaders/fragment_shader.glsl");
  GLuint vertexShader = compile_shader(vertexShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShader =
      compile_shader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  // Cria o programa de shader
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Verifica erros de linkagem
  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "Erro na linkagem do programa de shader: %s\n", infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  free(vertexShaderSource);
  free(fragmentShaderSource);

  // Configura os dados dos vértices (um cubo)
  float vertices[] = {
      // Posições          // Coordenadas de textura
      // Frente
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // Inferior esquerdo
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // Inferior direito
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,    // Superior direito
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,   // Superior esquerdo
      // Traseira
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  // Inferior esquerdo
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,   // Inferior direito
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,    // Superior direito
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   // Superior esquerdo
      // Esquerda
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  // Inferior esquerdo
      -0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // Inferior direito
      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f,    // Superior direito
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,   // Superior esquerdo
                                        // Direita
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f,   // Inferior esquerdo
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    // Inferior direito
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,     // Superior direito
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f,    // Superior esquerdo
      // Superior
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,   // Inferior esquerdo
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // Inferior direito
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // Superior direito
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // Superior esquerdo
      // Inferior
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // Inferior esquerdo
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,    // Inferior direito
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,   // Superior direito
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // Superior esquerdo
  };

  unsigned int indices[] = {
      // Frente
      0,
      1,
      2,
      2,
      3,
      0,
      // Traseira
      4,
      5,
      6,
      6,
      7,
      4,
      // Esquerda
      8,
      9,
      10,
      10,
      11,
      8,
      // Direita
      12,
      13,
      14,
      14,
      15,
      12,
      // Superior
      16,
      17,
      18,
      18,
      19,
      16,
      // Inferior
      20,
      21,
      22,
      22,
      23,
      20,
  };

  // Configura buffers e arrays de vértices
  glGenVertexArrays(1, &VAO);
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    fprintf(stderr, "Erro OpenGL (glGenVertexArrays): %d\n", error);
  }
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // Vincula VAO
  glBindVertexArray(VAO);

  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Atributos de vértice
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Carrega texturas
  const char* texture_files[] = {"assets/textures/grass.png",
                                 "assets/textures/dirt.png",
                                 "assets/textures/stone.png"};
  glGenTextures(3, textures);
  for (int i = 0; i < 3; i++) {
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, nrChannels;
    unsigned char* data =
        stbi_load(texture_files[i], &width, &height, &nrChannels, 0);
    if (data) {
      GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      fprintf(stderr, "Falha ao carregar a textura %s\n", texture_files[i]);
    }
    stbi_image_free(data);
  }

  // Define os uniforms das texturas
  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "grassTexture"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "dirtTexture"), 1);
  glUniform1i(glGetUniformLocation(shaderProgram, "stoneTexture"), 2);

  // Ativa e vincula texturas às unidades de textura correspondentes
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);  // grassTexture

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);  // dirtTexture

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, textures[2]);  // stoneTexture

  glUseProgram(0);

  // Habilita teste de profundidade
  glEnable(GL_DEPTH_TEST);
}

void renderer_clear() {
  // Limpa o buffer de cor e profundidade
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_draw_world() {
  // Usa o programa de shader
  glUseProgram(shaderProgram);

  // Configura as matrizes de visualização e projeção
  GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

  // Obter a matriz de visão do jogador
  mat4 view_matrix;
  player_get_view_matrix((float*)view_matrix);

  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view_matrix);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, camera_get_projection_matrix());

  // Obtém a posição atual do jogador para determinar os chunks visíveis
  vec3 player_position;
  player_get_position(player_position);

  int player_chunk_x = (int)(player_position[0] / CHUNK_WIDTH);
  int player_chunk_z = (int)(player_position[2] / CHUNK_DEPTH);

  // Define o alcance de renderização ao redor do jogador
  const int render_distance = 2;

  for (int cx = player_chunk_x - render_distance;
       cx <= player_chunk_x + render_distance; cx++) {
    for (int cz = player_chunk_z - render_distance;
         cz <= player_chunk_z + render_distance; cz++) {
      Chunk* chunk = world_get_chunk(cx, cz);
      if (!chunk) continue;  // Pula chunks não carregados

      // Verifica se o chunk precisa ser atualizado
      if (chunk->needs_update) {
        chunk_update_mesh(chunk);  // Atualiza a malha do chunk
      }

      // Pula chunks vazios (sem índice para renderizar)
      if (chunk->index_count == 0) continue;

      // Define a matriz modelo para o chunk
      mat4 model;
      glm_mat4_identity(model);
      glm_translate(model, (vec3){cx * CHUNK_WIDTH, 0.0f, cz * CHUNK_DEPTH});
      GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);

      // Vincula o VAO do chunk e desenha
      glBindVertexArray(chunk->vao);

      // Seleciona a textura correta (assumindo uma textura única no momento)
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textures[0]);  // Ajuste se necessário

      // Renderiza o chunk
      glDrawElements(GL_TRIANGLES, chunk->index_count, GL_UNSIGNED_INT, 0);

      // Desvincula o VAO para evitar problemas futuros
      glBindVertexArray(0);
    }
  }

  // Desvincula o programa de shader
  glUseProgram(0);
}

void renderer_cleanup() {
  // Limpa recursos de renderização
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);
}

void renderer_reset() {
  renderer_cleanup();
  renderer_init();
}
