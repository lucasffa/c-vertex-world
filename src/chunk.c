// src/chunk.c

#include "chunk.h"

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

Chunk* chunk_create(int x, int z) {
  Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));
  chunk->x = x;
  chunk->z = z;

  // Inicializa os blocos (exemplo simples)
  for (int i = 0; i < CHUNK_WIDTH; i++) {
    for (int j = 0; j < CHUNK_HEIGHT; j++) {
      for (int k = 0; k < CHUNK_DEPTH; k++) {
        if (j < 20) {
          chunk->blocks[i][j][k].type = BLOCK_STONE;
        } else if (j < 25) {
          chunk->blocks[i][j][k].type = BLOCK_DIRT;
        } else if (j == 25) {
          chunk->blocks[i][j][k].type = BLOCK_GRASS;
        } else {
          chunk->blocks[i][j][k].type = BLOCK_AIR;
        }
      }
    }
  }

  // **Adicione esta parte para gerar os buffers**
  glGenVertexArrays(1, &chunk->vao);
  glGenBuffers(1, &chunk->vbo);
  glGenBuffers(1, &chunk->ebo);

  chunk->index_count = 0;
  chunk->needs_update = 1;  // Marcar como precisando de atualização inicial

  return chunk;
}

void chunk_destroy(Chunk* chunk) {
  // Limpa os buffers do OpenGL
  glDeleteVertexArrays(1, &chunk->vao);
  glDeleteBuffers(1, &chunk->vbo);
  glDeleteBuffers(1, &chunk->ebo);

  free(chunk);
}

Block* chunk_get_block(Chunk* chunk, int x, int y, int z) {
  if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 &&
      z < CHUNK_DEPTH) {
    return &(chunk->blocks[x][y][z]);
  }
  return NULL;
}

void chunk_update_mesh(Chunk* chunk) {
  typedef struct {
    float position[3];
    float tex_coords[2];
    int block_type;
  } Vertex;

  size_t max_blocks = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH;
  size_t max_faces = max_blocks * 6;
  size_t max_vertices = max_faces * 4;
  size_t max_indices = max_faces * 6;

  Vertex* vertices = malloc(max_vertices * sizeof(Vertex));
  unsigned int* indices = malloc(max_indices * sizeof(unsigned int));
  if (!vertices || !indices) {
    fprintf(stderr, "Erro: Falha ao alocar memória para mesh.\n");
    free(vertices);
    free(indices);
    return;
  }

  size_t vertex_count = 0, index_count = 0;

  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_DEPTH; z++) {
        Block* block = &chunk->blocks[x][y][z];
        if (block->type == BLOCK_AIR) continue;

        // Para cada face, você verifica se precisa ser renderizada e adiciona
        // os vértices e índices Face frontal (Z+)
        if (z == CHUNK_DEPTH - 1 ||
            chunk->blocks[x][y][z + 1].type == BLOCK_AIR) {
          Vertex v1 = {{x, y, z + 1}, {0.0f, 0.0f}, block->type};
          Vertex v2 = {{x + 1, y, z + 1}, {1.0f, 0.0f}, block->type};
          Vertex v3 = {{x + 1, y + 1, z + 1}, {1.0f, 1.0f}, block->type};
          Vertex v4 = {{x, y + 1, z + 1}, {0.0f, 1.0f}, block->type};

          vertices[vertex_count++] = v1;
          vertices[vertex_count++] = v2;
          vertices[vertex_count++] = v3;
          vertices[vertex_count++] = v4;

          unsigned int base_index = vertex_count - 4;
          indices[index_count++] = base_index;
          indices[index_count++] = base_index + 1;
          indices[index_count++] = base_index + 2;
          indices[index_count++] = base_index;
          indices[index_count++] = base_index + 2;
          indices[index_count++] = base_index + 3;
        }

        // Repita o mesmo para as outras faces...

        // Verifique se não ultrapassou a capacidade
        if (vertex_count + 4 > max_vertices || index_count + 6 > max_indices) {
          fprintf(
              stderr,
              "Erro: Excedeu a capacidade máxima de vértices ou índices.\n");
          free(vertices);
          free(indices);
          return;
        }
      }
    }
  }

  // Upload para a GPU
  glBindVertexArray(chunk->vao);
  glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int),
               indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, tex_coords));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  chunk->index_count = index_count;

  free(vertices);
  free(indices);

  chunk->needs_update = 0;
}
