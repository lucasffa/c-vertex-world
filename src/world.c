// src/world.c

#include "world.h"

#include <stdlib.h>

#define WORLD_SIZE 3  // Mundo de 3x3 chunks

static Chunk* chunks[WORLD_SIZE][WORLD_SIZE];

void world_init() {
  // Inicializa os chunks
  for (int x = 0; x < WORLD_SIZE; x++) {
    for (int z = 0; z < WORLD_SIZE; z++) {
      chunks[x][z] = chunk_create(x, z);
    }
  }
}

void world_cleanup() {
  // Limpa os chunks
  for (int x = 0; x < WORLD_SIZE; x++) {
    for (int z = 0; z < WORLD_SIZE; z++) {
      chunk_destroy(chunks[x][z]);
    }
  }
}

Chunk* world_get_chunk(int x, int z) {
  // Retorna o chunk na posição especificada
  if (x >= 0 && x < WORLD_SIZE && z >= 0 && z < WORLD_SIZE) {
    return chunks[x][z];
  }
  return NULL;
}

Block* world_get_block(int x, int y, int z) {
  int chunk_x = x / CHUNK_WIDTH;
  int chunk_z = z / CHUNK_DEPTH;

  if (x < 0) chunk_x--;
  if (z < 0) chunk_z--;

  Chunk* chunk = world_get_chunk(chunk_x, chunk_z);
  if (!chunk) {
    return NULL;
  }

  int local_x = x % CHUNK_WIDTH;
  int local_z = z % CHUNK_DEPTH;

  if (local_x < 0) local_x += CHUNK_WIDTH;
  if (local_z < 0) local_z += CHUNK_DEPTH;

  if (y < 0 || y >= CHUNK_HEIGHT) {
    return NULL;
  }

  return chunk_get_block(chunk, local_x, y, local_z);
}

void world_set_block(int x, int y, int z, BlockType type) {
  int chunk_x = x / CHUNK_WIDTH;
  int chunk_z = z / CHUNK_DEPTH;

  if (x < 0 && x % CHUNK_WIDTH != 0) chunk_x--;
  if (z < 0 && z % CHUNK_DEPTH != 0) chunk_z--;

  Chunk* chunk = world_get_chunk(chunk_x, chunk_z);
  if (!chunk) return;

  int local_x = x % CHUNK_WIDTH;
  int local_z = z % CHUNK_DEPTH;

  if (local_x < 0) local_x += CHUNK_WIDTH;
  if (local_z < 0) local_z += CHUNK_DEPTH;

  if (y < 0 || y >= CHUNK_HEIGHT) return;

  Block* block = chunk_get_block(chunk, local_x, y, local_z);
  block->type = type;

  // Marca o chunk como precisando de atualização
  chunk->needs_update = 1;
}
