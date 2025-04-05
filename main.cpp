#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "console.h" 
#include <string.h>
#include <chrono>
#include <thread>

// Globals

const uint8_t ROWS = 20;
const uint8_t COLS = 20;
const int     BOARD_SIZE = ROWS * COLS;

// Functions

int  countNeighbours (const char* BOARD, int i);
void update          (char * BOARD);

// Main code

int main (void) {
	clear();

	int CELLS;

	char BOARD[BOARD_SIZE + 1];
	memset(BOARD, '.', BOARD_SIZE); // Set the BOARD to be just dots (dead cell)
	BOARD[BOARD_SIZE] = '\0';
	#if LINUX	
	printf("%s", GREEN);
	#elif WIN
	setColour(WIN_GREEN);
	#endif
	printf("\t\t\t\tCONWAYS GAME OF LIFE v1\n");
	printf("\t\t\t\t\t- https://github.com/TierTheTora\n\n\n");
	#if LINUX
	printx(false, true, true, false, "RULES\n");
	#elif WIN
	printf("RULES\n");
	#endif
	printf(" 1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation. \n \
2. Any live cell with two or three live neighbours lives on to the next generation. \n \
3. Any live cell with more than three live neighbours dies, as if by overpopulation. \n \
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.\n\n");
	printf("Number of Cells: ");

	std::cin >> CELLS;

	if (CELLS > BOARD_SIZE) { // If there are more cells than cells available
		#if LINUX
		printf("%s", RED);
		#elif WIN
		setColour(WIN_RED);
		#endif
		fprintf(stderr, "Number of Cells can not be greater than %d.\n", BOARD_SIZE);
		exit(1);
	}

	for (int i = 0; i < CELLS; ++i) { // Ask for cell position `CELL` amount of times
		int COORD;

		printf("Grid position of Cell #%d: ", i+1);
		std::cin >> COORD;

		BOARD[COORD-1] = '#'; // Set the baord at the user input to be an alive cell 
	
		for (int j = 0; j < BOARD_SIZE; ++j) { // Iterate through each cell
			#if LINUX
			printf("%s", GREY);
			if (BOARD[j] == '#') printf("%s", YELLOW);
			#elif WIN
			setColour(WIN_GREY);
			if (BOARD[j] == '#') setColour(WIN_YELLOW);
			#endif
			printf("%c ", BOARD[j]);
			#if LINUX
			printf("%s", GREY);
			#elif WIN
			setColour(WIN_GREY);
			#endif
			if ((j + 1) % ROWS == 0) { // If we are at the end of the first row
				printf("\n");
			}
		}
		#if LINUX
		printf("%s", GREEN);
		#elif WIN
		setColour(WIN_GREEN);
		#endif
		printf("\n");
	}

	char conf[1];
	printf("Ready? (y/n) "); // Game start confirmation
	std::cin >> conf;
	if (tolower(conf[0]) == 'y') {
		update(BOARD);
	}
	else exit(0);
	#if LINUX
	printf("%s", RESET);
	#elif WIN
	setColour(WIN_RESET);
	#endif
}

void update (char * BOARD) {
	clear();
	char newBoard[BOARD_SIZE + 1]; // Create the next board generation
	newBoard[BOARD_SIZE] = '\0';

	while (true) {
		for (int i = 0; i < BOARD_SIZE; ++i) { // Loop through each cell
			int neighbours = countNeighbours(BOARD, i); // Get the neighbouring cells of the current cell
			if (BOARD[i] == '#') { // If the cell is alive
				newBoard[i] = (neighbours == 2 || neighbours == 3) ? '#' : '.';  // If there are 2 or 3 neighbouring cellswe set the current cell to be alive
			} 
			else { // If the current cell is dead 
				newBoard[i] = (neighbours == 3) ? '#' : '.'; // If there are 3 neighbouring cells we set the current cell to be alive
			}
		}
		memcpy(BOARD, newBoard, BOARD_SIZE); // Set the board to the next generation
		clear();
		for (int j = 0; j < BOARD_SIZE; ++j) { // Write the new board to the screen
			#if LINUX
			printf("%s", GREY);
			if (BOARD[j] == '#') printf("%s", YELLOW);
			#elif WIN
			setColour(WIN_GREY);
			if (BOARD[j] == '#') setColour(WIN_YELLOW);
			#endif
			printf("%c ", BOARD[j]);
			#if LINUX
			printf("%s", GREY);
			#elif WIN
			setColour(WIN_GREY);
			#endif
			if ((j + 1) % ROWS == 0) {
				printf("\n");
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait 1 second
	}
}

int countNeighbours (const char * BOARD, int i) {
	int row = i / COLS; // Get the current row
	int col = i % COLS; // Get the current column
	int count = 0; // Neighbouring cell count

	for (int dr = -1; dr <= 1; ++dr) { // Check surrounding rows
		for (int dc = -1; dc <= 1; ++dc) { // Check surrounding columns
			if (dr == 0 && dc == 0) continue; // Skip the current cell
			int r = row + dr; // Row the neighour is at
			int c = col + dc; // Column the neighbour is at
			if (r >= 0 && r < ROWS && c >= 0 && c < COLS) { // Check if the neighbour is inside the board
				if (BOARD[r * COLS + c] == '#') { // If the cell is alive increase the neighbour count
					++count;
				}
			}
		}
	}
	return count;
}
