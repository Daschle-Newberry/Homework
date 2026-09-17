#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include "gameOfLifeOutput.h"


typedef struct world_t {
    unsigned short height, width;   // < Board height/width
	char* board;                    // < Board of booleans, size height * width
} world_t;

/**
 * Creates new world from seed
 * @param seed Pointer to file containing seed 
 * @return Pointer to world, NULL pointer if failure
 */

world_t* gol_create_world(FILE* seed);

/**
 * Frees world and board
 * @param world World to be freed
 * @return void
 */
void gol_free_world(world_t* world);

/**
 * Simulates n generations of Conway's game of life on the given world
 * 
 * @param world World with initial state
 * @param count Number of generations to simulate
 * @param vertical_wrap Flag for if the top and bottom are adjacent
 * @param horizontal_wrap Flag for if the left and right are adjacent
 * @param output_format Null terminated string for the base format of output files
 * @return int indicating success, 0 for success 1 for failure
 */
int gol_simulate(world_t* world, int count, int vertical_wrap, int horizontal_wrap, char* output_format);

#endif
