// src/block.h

#ifndef BLOCK_H
#define BLOCK_H

typedef enum {
    BLOCK_AIR,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_STONE
} BlockType;

typedef struct {
    BlockType type;
} Block;

#endif // BLOCK_H
