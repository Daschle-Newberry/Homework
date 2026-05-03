#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "gameOfLife.h"
#include "gameOfLifeOutput.h"


#define MOD(a,n) ((a + n) % n)

world_t* gol_init_world(unsigned short height, unsigned short width) {
    world_t* world = malloc(sizeof(world_t));

    if(world == NULL) {
        return NULL;
    }

    world->board = malloc(sizeof(char) * height * width);

    if(world->board == NULL) {
        free(world);
        return NULL;
    }

    world->height = height;
    world->width = width;

    return world;
}

void gol_free_world(world_t* world) {
    free(world->board);
    free(world);
}

world_t* gol_create_world(FILE* seed) {
    if(seed == NULL) return NULL;
    short int rows, cols;

    if(fscanf(seed, " %hd", &rows) != 1 || rows < 1) {
        fprintf(stderr, "Invalid or missing number of rows in seed file\n");
        return NULL;
    }

    if(fscanf(seed, " %hd", &cols) != 1 || cols < 1) {
        fprintf(stderr, "Invalid or missing number of columns in seed file\n");
        return NULL;
    }

    world_t* world = gol_init_world(rows, cols);

    if(world == NULL) {
        return NULL;
    }

    for(short int r = 0; r < rows; r++) {
        char row[cols];

        char cell;

        short int c = 0;
        while(c < cols && (cell = fgetc(seed)) != EOF) {
            if(isspace(cell)) continue;
            // if(!(cell == '.' || cell == '0')) break;

            row[c] = (char) cell == '.' ? 0 : 1;
            c++;
        }

        if(c < cols) {
            fprintf(stderr, "Invalid seed\n");
            gol_free_world(world);
            return NULL;
        }

        memcpy(&world->board[r * world->width], row, cols * sizeof(char));
    }

    return world;
}

static int get_num_neighbors(world_t* world,
                             int x, int y,
                             int vertical_wrap, int horizontal_wrap
                            )
    {
    int num_neighbors = 0;
    for(short int dy = -1; dy <= 1; dy++) {
        for(short int dx = -1; dx <= 1; dx++) {
            if(dy == 0 && dx == 0) continue;

            short int cell_y = y + dy;
            short int cell_x = x + dx;

            if( !vertical_wrap && (cell_y < 0 || world->height - 1 < cell_y)) continue;
            if( !horizontal_wrap && (cell_x < 0 || world->width - 1 < cell_x)) continue;

            cell_y = MOD(cell_y, world->height);
            cell_x = MOD(cell_x, world->width);

            int idx = (cell_y * world->width) + cell_x;
            if(world->board[idx] == 1) num_neighbors++;
        }
    }

    return num_neighbors;
}

static void write_out(golo_manager_t* manager, char* output_format, int gen) {
    char output_name[strlen(output_format) + 10];
    snprintf(output_name, sizeof(output_name), "%s_%d", output_format, gen);

    FILE* gen_output = fopen(output_name,"w");
    golo_write_text(manager, gen_output);
    fclose(gen_output);
}

static inline void copy_to_manager(golo_manager_t* manager, world_t* world) {
    for(int i = 0; i < world->height; i++) golo_set_row(manager, i, &world->board[i * world->width]);
}

int gol_simulate(world_t* world, unsigned int count, int vertical_wrap, int horizontal_wrap, char* output_format) {
    if(world == NULL) return 1;

    golo_manager_t* manager = golo_init(world->height, world->width);

    if(manager == NULL) {
        return 1;
    }

    copy_to_manager(manager, world);
    write_out(manager, output_format, 0);

    unsigned int gen = 1;
    while(gen <= count) {
        char new_world[world->height][world->width];

        for(short int y = 0; y < world->height; y++) {
            for(short int x = 0; x < world->width; x++) {
                int num_neighbors = get_num_neighbors(
                            world,
                            x,
                            y,
                            vertical_wrap,
                            horizontal_wrap
                            );
                if(num_neighbors < 2 || 3 < num_neighbors) new_world[y][x] = 0;
                else if(num_neighbors == 3) new_world[y][x] = 1;
                else new_world[y][x] = world->board[(y * world->width) + x];
            }
        }
        memcpy(world->board, new_world, sizeof(new_world));
        copy_to_manager(manager, world);
        write_out(manager, output_format, gen);

        gen++;
    }


    golo_free(manager);
    return 0;
}
