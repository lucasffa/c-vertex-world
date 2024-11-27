// src/world.h

#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"

void world_init();
void world_cleanup();
Chunk* world_get_chunk(int x, int z);
Block* world_get_block(int x, int y, int z);
void world_set_block(int x, int y, int z, BlockType type);

#endif  // WORLD_H
