#include <stdio.h>
#include <string.h>

#include "gameOfLife.h"



int main(int argc, char* argv[]) {
    int num_gen;
    int vertical_wrap = 0;
    int horizontal_wrap = 0;
    FILE* seed;

    // Arg 1 does not exist or is not an integer greater than or equal to 0
    if(argc < 2 || sscanf(argv[1], "%d", &num_gen) != 1 || num_gen < 0) {
        fprintf(stderr,"Invalid or missing number of generations\n");
        return 1;
    }

    // Arg 2 does not exist or is an invalid path
    if(argc < 3 || (seed = fopen(argv[2], "r")) == NULL) {
       fprintf(stderr, "Invalid or missing seed input file\n");
       return 1;
    }

    // Arg 3 exists but either is not a number or is not 0 or 1
    if(argc >= 4 &&
       (strlen(argv[3]) > 1 || sscanf(argv[3], "%d", &vertical_wrap) != 1 ||
       (vertical_wrap != 0 && vertical_wrap != 1)))
    {
        fprintf(stderr, "Invalid argument for vertical wrap\n");
        fclose(seed);
        return 1;
    }

    // Arg 4 exists but either is not a number or is not 0 or 1
    if(argc >= 5 &&
       (strlen(argv[4]) > 1 || sscanf(argv[4], "%d", &horizontal_wrap) != 1 ||
       (horizontal_wrap != 0 && horizontal_wrap != 1)))
    {
        fprintf(stderr, "Invalid argument for horizontal wrap\n");
        fclose(seed);
        return 1;
    }

    printf("Arguments: \n   Generations: %d\n   Seed: %s\n   Vertical wrap: %d\n   Horizontal wrap: %d\n",
           num_gen, argv[2], vertical_wrap, horizontal_wrap);
    
    world_t* world = gol_create_world(seed);
    fclose(seed);
    if(world == NULL) return 1;

    gol_simulate(world, num_gen, vertical_wrap, horizontal_wrap, argv[2]);
    gol_free_world(world);

    return 0;
}
