#include "gameOfLifeOutput.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// allocate and initialize a new output manager for a world of given size
golo_manager_t * golo_init(unsigned short height, unsigned short width) {
	golo_manager_t * newbie;

	newbie = (golo_manager_t *)calloc(1, sizeof(golo_manager_t));
	if(!newbie) { return NULL; } // couldn't get memory for struct
	
	newbie->world = (char *)calloc(height * width, sizeof(char));
	if(!newbie->world) {
		free(newbie);
		return NULL;
	}

	newbie->world_height = height;
	newbie->world_width = width;

	return newbie;
}

// release a manager
void golo_free(golo_manager_t * m) {
	free(m->world);
	free(m);
}

// update one row
void golo_set_row(golo_manager_t *m, unsigned short row_num, char * row) {
	unsigned short width = m->world_width;
	for(int c = 0; c < width; c++) {
		m->world[row_num * width + c] = row[c];
	}
}

// write current manager state into given file as ASCII
void golo_write_text(golo_manager_t *m, FILE * fh) {
	unsigned short width = m->world_width;
	unsigned short height = m->world_height;

	// write dimensions
	fprintf(fh, "%d %d\n", height, width);

	// loop through all cells and print their contents
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			// we use 0 and 1 internally, so we can just treat that as boolean
			if(m->world[r * width + c])
				{ putc('0', fh); }
			else
				{ putc('.', fh); }
		}
		putc('\n', fh);
	}
}

// given a (positive) int, round it up to the nearest multiple of 4
static int round4(int n) {
	return n % 4 ? n - (n % 4) + 4 : n;
}

// write current manager state into given file as a BMP
int golo_write_bmp(golo_manager_t *m, FILE * fh) {
	// NOTE: details for writing a BMP are from: https://lmcnulty.me/words/bmp-output/
	unsigned short width = m->world_width;
	unsigned short height = m->world_height;

	/**
	 * Dimensions of BMP_CELL_SIZE + BMP_LINE_WIDTH pixels per cell, 
	 * plus one extra line width to complete the border
	 */
	int pixel_width = width * (BMP_CELL_SIZE + BMP_LINE_WIDTH) + BMP_LINE_WIDTH;
	int pixel_height = height * (BMP_CELL_SIZE + BMP_LINE_WIDTH) + BMP_LINE_WIDTH;

	// figure out appropriate header
	char tag[] = {'B', 'M'};
	int header[] = {
        0x3a, 0x00, 0x36,
        0x28,                // Header Size
        pixel_width, pixel_height,       // Image dimensions in pixels
        0x180001,            // 24 bits/pixel, 1 color plane
        0,                   // BI_RGB no compression
        0,                   // Pixel data size in bytes (Only need for compressed data)
        0x002e23, 0x002e23,  // Print resolution ~~ 300DPI
        0, 0                 // No color palette
    };

	/**
	 * BMP files need each row of pixels to be a multiple of 4 bytes,
	 * and each pixel needs 3 bytes (red/green/blue). So, work out how wide
	 * a row needs to be:
	 */
	int padded_width = round4(pixel_width * 3);
	int total_size = padded_width * pixel_height;

	// Allocate a big 'ol buffer to hold all of the pixel data
	char *pixel_data = (char *)calloc(total_size, sizeof(char));
	if(!pixel_data) { return 0; }

	/**
	 * generate the pixel data. This is a little ticklish, since
	 * our world data has the top row first, but the BMP needs the top row last.
	 */
	// line of pixels at the top of the image (all 127 will give medium grey )
	for(int r = 0; r < BMP_LINE_WIDTH; r++) {
		for(int c = 0; c < pixel_width * 3; c++) 
			{ pixel_data[(pixel_height - 1 - r) * padded_width + c] = 127; }
	}

	// generate remaining rows
	for(int row = 0; row < height; row++) {
		// where the row begins in the bitmap's array
		int bmp_row = (height - 1 - row) * (BMP_CELL_SIZE + BMP_LINE_WIDTH);

		// line below row
		for(int r = 0; r < BMP_LINE_WIDTH; r++) {
			for(int c = 0; c < pixel_width * 3; c++)
				{ pixel_data[(bmp_row) * padded_width + c] = 127; }
			bmp_row++; // count up any time you complete a row of pixels
		}

		// row pixels
		for(int row_repeat = 0; row_repeat < BMP_CELL_SIZE; row_repeat++) {
			int bmp_col = 0; // where we are in the column

			// note: everying is * 3 because 3 colors / pixel
			// initial line
			for(int col = 0; col < BMP_LINE_WIDTH * 3; col++) 
				{ pixel_data[bmp_row * padded_width + bmp_col++] = 127; }

			// each individual cell
			for(int col = 0; col < width; col++) {
				// draw black if cell is occupied (1) otherwise white
				for(int col_repeat = 0; col_repeat < BMP_CELL_SIZE * 3; col_repeat++) {
						pixel_data[bmp_row * padded_width + bmp_col++] = (1 - m->world[row * width + col]) * 255; 
				}

				// Line after the cell
				for(int col_repeat = 0; col_repeat < BMP_LINE_WIDTH * 3; col_repeat++) 
					{ pixel_data[bmp_row * padded_width + bmp_col++] = 127; }
			}
			bmp_row++;
		}
	}

	fwrite(&tag, sizeof(tag), 1, fh);
    fwrite(&header, sizeof(header), 1, fh);
	fwrite(pixel_data, total_size, 1, fh);

	free(pixel_data);
	return 1;
}

// figure out how tall the terminal window is and return it
int golo_get_screen_height(void) {
    int lines = 24;

	/**
	 * this next bit basically asks the system
	 * how many lines of text fit onscreen / in the terminal window
	 * 
	 * via
	 * https://www.linuxquestions.org/questions/programming-9/get-width-height-of-a-terminal-window-in-c-810739/
	 */
	#ifdef TIOCGSIZE
		struct ttysize ts;
		ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
		lines = ts.ts_lines;
	#elif defined(TIOCGWINSZ)
		struct winsize ts;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
		lines = ts.ws_row;
	#endif 

	return lines;
}

void golo_clear_screen(void) {
    int lines = golo_get_screen_height(); // get height of screen
    while(lines--) { putchar('\n'); } // print exactly enough lines
}

void golo_pad_screen(golo_manager_t *m) {
	int lines = golo_get_screen_height(); // get height of screen
	lines -= m->world_height + 1;
	if(lines > 0) {
		while(lines--) { putchar('\n'); } // print exactly enough lines
	}
}
