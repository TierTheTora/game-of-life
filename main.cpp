#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "console.h" 
#include <string.h>
#include <chrono>
#include <thread>

// Globals

const uint8_t ROWS = 10;
const uint8_t COLS = 10;
const int     SIZE = ROWS * COLS;

// Functions

int  countNeighbours (const char* BOARD, int i);
void update          (char * BOARD);

// Main code

int main (void) {
	clear();

	int CELLS;

	char BOARD[SIZE + 1];
	memset(BOARD, '.', SIZE); // Set the BOARD to be just dots (dead cell)
	BOARD[SIZE] = '\0';
	
	printf("%s", GREEN);
	printf("\t\t\t\tCONWAYS GAME OF LIFE v1\n");
	printf("\t\t\t\t\t- https://github.com/TierTheTora\n\n\n");
	printx(false, true, true, false, "RULES\n");
	printf(" 1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation. \n \
2. Any live cell with two or three live neighbours lives on to the next generation. \n \
3. Any live cell with more than three live neighbours dies, as if by overpopulation. \n \
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.\n\n");
	printf("Number of Cells: ");

	std::cin >> CELLS;

	if (CELLS > SIZE) {
		printf("%s", RED);
		fprintf(stderr, "Number of Cells can not be greater than %d.\n", SIZE);
		exit(1);
	}

	for (int i = 0; i < CELLS; ++i) {
		int COORD;
		printf("Grid position of Cell #%d: ", i+1);
		std::cin >> COORD;
		BOARD[COORD-1] = '#'; // Set the baord at the user input to be an alive cell 
		for (int j = 0; j < SIZE; ++j) {
			printf("%s", GREY);
			if (BOARD[j] == '#') printf("%s", YELLOW);
			printf("%c ", BOARD[j]);
			printf("%s", GREY);
			if ((j + 1) % ROWS == 0) {
				printf("\n");
			}
		}
		printf("%s%s", BRESET, GREEN);
		printf("\n");
	}

	char conf[1];
	printf("Ready? (y/n) ");
	std::cin >> conf;
	if (tolower(conf[0]) == 'y') {
		update(BOARD);
	}
	else exit(0);

	printf("%s%s", BRESET, RESET);
}

void update (char * BOARD) {
	clear();
	char newBoard[SIZE + 1];
	newBoard[SIZE] = '\0';

	while (true) {
		for (int i = 0; i < SIZE; ++i) {
			int neighbours = countNeighbours(BOARD, i);
			if (BOARD[i] == '#') {
				newBoard[i] = (neighbours == 2 || neighbours == 3) ? '#' : '.';
			} 
			else {
				newBoard[i] = (neighbours == 3) ? '#' : '.';
			}
		}
		memcpy(BOARD, newBoard, SIZE);
		clear();
		for (int j = 0; j < SIZE; ++j) {
			printf("%s", GREY);
			if (BOARD[j] == '#') printf("%s", YELLOW);
			printf("%c ", BOARD[j]);
			printf("%s", GREY);
			if ((j + 1) % ROWS == 0) {
				printf("\n");
			}
		}
		printf("%s", BRESET);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int countNeighbours (const char * BOARD, int i) {
	int row = i / COLS;
	int col = i % COLS;
	int count = 0;

	for (int dr = -1; dr <= 1; ++dr) {
		for (int dc = -1; dc <= 1; ++dc) {
			if (dr == 0 && dc == 0) continue;
			int r = row + dr;
			int c = col + dc;
			if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
				if (BOARD[r * COLS + c] == '#') {
					++count;
				}
			}
		}
	}
	return count;
}
