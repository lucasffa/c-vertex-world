// src/chunk.h

#ifndef CHUNK_H
#define CHUNK_H

#include <GL/glew.h>

#include "block.h"

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 64
#define CHUNK_DEPTH 32

typedef struct {
  int x, z;
  Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
  int needs_update;      // Flag para indicar se o chunk precisa ser atualizado
  GLuint vao, vbo, ebo;  // Buffers de renderização
  int index_count;       // Número de índices para desenhar
} Chunk;

Chunk* chunk_create(int x, int z);
void chunk_destroy(Chunk* chunk);
Block* chunk_get_block(Chunk* chunk, int x, int y, int z);
void chunk_update_mesh(Chunk* chunk);

#endif  // CHUNK_H
