#ifndef GAMEOFLIFEOUTPUT_H
#define GAMEOFLIFEOUTPUT_H

#include <stdio.h>

/**
 * This library provides support for writing the current state of a
 * Game of Life 'world' to a file, either in text or as an image.
 *
 * To accomplish this, it maintains its own snapshot of the world
 * state which can be updated by the user of the library. This
 * state is for use by the library only, and is not set up to be
 * used by anything other than the library itself. Notably, it is
 * not designed to be read by library users, nor should it be updated
 * except via library functions.
 * 
 * Basic usage is as follows:
 * 	1) Use golo_init() to create a new world state object
 * 	2) Use golo_set_row() to set state of all rows in the world
 *  3) Use one of the output functions to write data to a file
 *  4) Repeat 2 and 3 as many times as needed
 * 	5) Use golo_free() to release memory allocated by the library
 * 
 * Note: 'golo' is an acronym of 'Game of Life Output'
 */

/**
 * You may modify this file (and the matching C file), but Autolab
 * will clobber your version and replace it with mine for testing.
 */

// You're welcome to adjust the following for your own purposes
#define BMP_CELL_SIZE 16 // number of pixels used to represent one cell in a BMP
#define BMP_LINE_WIDTH 1 // width of the lines separating cells, in pixels

/**
 * this structure is for use by this library ONLY!
 * DO NOT mess with its contents directly
 */
typedef struct {
	unsigned short world_height, world_width;
	char *world;
} golo_manager_t;

/**
 * Initialize a new output manager for a world of given size
 * 
 * Stored world is initially empty. Call golo_set_row to populate it.
 * 
 * Receives:
 * 		height, width: Dimensions of the world, in cells
 *
 * Returns:
 * 		Initialized world, or NULL if allocation fails.
 * 		BE SURE TO CHECK THE RETURN VALUE!
 */
golo_manager_t * golo_init(unsigned short height, unsigned short width);

/**
 * Frees all memory associated with the world state, ala free().
 */
void golo_free(golo_manager_t * m);

/**
 * Set the contents of a single row in the manager's world state, replacing the
 * previous state.
 * 
 * Receives:
 * 		m: a pointer to an initialized golo_manager_t
 * 		row_num: row in the world to update, starting from the top, 0 indexed
 * 		row: character array populated with the numbers 0 or 1 indicating
 * 			 whether a given cell is occupied. Assumed to be long enough
 */
void golo_set_row(golo_manager_t *m, unsigned short row_num, char * row);

/**
 * Write the current (manager) world state to a file using a text representation.
 * The output complies with the input specification for the Game of Life assignment:
 *
 * The height and width are printed in decimal form on a single line,
 * whitespace separated. This is followed on the next line by the world.
 * The output uses a . (period) to represent a blank cell,
 * and a 0 (zero) to represent an occupied cell. Individual rows of output
 * are separated by a newline although THIS IS NOT REQUIRED BY THE INPUT FORMAT.
 * 
 * Receives:
 * 		m: pointer to the manager to output
 * 		fh: a file handled open for writing to which the state should be written
 * 
 * Note: You could pass stdout to this function to write to the screen...
 */
void golo_write_text(golo_manager_t *m, FILE * fh);

/**
 * Write the current (manager) world state to a file as an image.
 * 
 * For readability, the output uses more than one pixel per cell,
 * and draws a grid between the cells.
 * 
 * Receives:
 * 		m: pointer to the manager to output
 * 		fh: a file handled open for writing to which the state should be written
 * 
 * Returns:
 * 		1 on a successful write, or 0 otherwise
 * 		If a write fails, the file is not modified.
 */
int golo_write_bmp(golo_manager_t *m, FILE * fh);

/**
 * returns the current height of the screen / terminal window in characters.
 * 
 * Defaults to 24 if it can't actually read a value, which is the default
 * for a *nix terminal.
 */
int golo_get_screen_height(void);

/**
 * 'Clears' the screen by writing enough empty lines to push any existing text 
 * off the top of the terminal. 
 * 
 * Perhaps you'd like to pair this with write_text() and usleep from <unistd.h>
 * to generate some terminal-based animation?
 */
void golo_clear_screen(void);

/**
 * 'Pad' the bottom of the screen with enough empty lines to push the output
 * from the manager to the top. Will add enough padding to hide the dimensions.
 * 
 * Perhaps you'd like to pair this with write_text() and usleep from <unistd.h>
 * to generate some terminal-based animation?
 */
void golo_pad_screen(golo_manager_t *m);

#endif
