#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include "gameOfLifeOutput.h"


typedef struct world_t {
    unsigned short height, width;
	char* board;
} world_t;

void gol_free_world(world_t* world);

world_t* gol_create_world(FILE* seed);

int gol_simulate(world_t* world, unsigned int count, int vertical_wrap, int horizontal_wrap, char* output_format);


#endif
