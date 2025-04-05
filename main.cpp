#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "console.h" 
#include <string.h>
#include <chrono>
#include <thread>
#include <regex>
#include <string>

// Globals

const uint8_t ROWS = 20;
const uint8_t COLS = 20;
const int     BOARD_SIZE = ROWS * COLS;

// Functions

int  countNeighbours (const char* BOARD, int i);
void update          (char * BOARD);
void displayBoard    (char * BOARD);

// Classes

class Command {
	public:
	void command(char * cmd, char * BOARD) {
		// Trim spaces
		int i = 0, j = 0;
		while (cmd[i]) {
			if (cmd[i] != ' ') {
				cmd[j++] = cmd[i];
			}
			i++;
		}
		cmd[j] = '\0';
		for (int k = 0; cmd[k]; ++k) { // Convert cmd to be lowercase
			cmd[k] = tolower(cmd[k]);
		}
		if (strcmp(cmd, "help") == 0) {
			#if LINUX
			printx(false, true, true, false, "RULES\n");
			#elif WIN
			printf("RULES\n");
			#endif
			printf(" 1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation. \n \
2. Any live cell with two or three live neighbours lives on to the next generation. \n \
3. Any live cell with more than three live neighbours dies, as if by overpopulation. \n \
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.\n\n");
			#if LINUX
			printx(false, true, true, false, "COMMANDS\n");
			#elif WIN
			printf("COMMANDS\n");
			#endif
			printf(" 1. +<int> : Make a cell alive on the board \n \ 
2. -<int> : Make a cell dead on the board \n \
3. board  : Show the current game board \n \
4. run    : Run the simulation\n\n");
		}
		else if (strcmp(cmd, "board") == 0) {
			displayBoard(BOARD);
		}
		else if (strcmp(cmd, "run") == 0) {
			update(BOARD);
		}
		{
			std::regex pattern("^\\+([0-9]+)$");
			std::smatch match;
			std::string cmdStr(cmd);
		
			if (std::regex_match(cmdStr, match, pattern)) {
				int number = std::stoi(match[1].str());
				if (number >= 0 && number < BOARD_SIZE) {
					BOARD[number-1] = '#';
				}
				else {
					printf("Number out of bounds.");
				}
			}
		}
		{
			std::regex pattern("^\\-([0-9]+)$");
			std::smatch match;
			std::string cmdStr(cmd);
		
			if (std::regex_match(cmdStr, match, pattern)) {
				int number = std::stoi(match[1].str());
				if (number >= 0 && number < BOARD_SIZE) {
					BOARD[number-1] = '.';
				}
				else {
					printf("Number out of bounds.");
				}
			}
		}
	}
};

// Main code

int main (void) {
	clear();


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

	while (true) { // Console prompt loop
		char cmd[1024];
		printf("> ");
		std::cin >> cmd;
		Command* command = new Command();
		command->command(cmd, BOARD);
		//displayBoard(BOARD);
	}

	
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
		displayBoard(BOARD);
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

void displayBoard (char * BOARD) {
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
	printf("\n");
	#if LINUX
	printf("%s", GREEN);
	#elif WIN 
	setColour(WIN_GREEN);
	#endif
}
