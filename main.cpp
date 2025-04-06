#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "console.h" 
#include <string.h>
#include <chrono>
#include <thread>
#include <regex>
#include <string>
#include <cstring>

// Globals

const uint8_t ROWS = 20;
const uint8_t COLS = 20;
const int     BOARD_SIZE = ROWS * COLS;

// Functions

int  countNeighbours (const char* BOARD, int i);
void update          (char * BOARD);
void displayBoard    (char * BOARD);

// Classes

namespace Command {
	void command(std::string cmd, char * BOARD) {
		// Trim spaces
		cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end()); // Removes the spaces
		// Convert cmd to be lowercase
		for (char& c : cmd) {
			c = std::tolower(c); 
		}
		if (cmd == "help") {
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
			printf(" 1. +<int>,<int> : Make a cell alive on the board \n \ 
2. -<int>,<int> : Make a cell dead on the board \n \
3. board        : Show the current game board \n \
4. run          : Run the simulation\n\n");
		}
		else if (cmd == "board") {
			displayBoard(BOARD);
		}
		else if (cmd == "run") {
			update(BOARD);
		}
		else if (cmd[0] == '+') {
			std::regex pattern("^\\+([0-9]+),([0-9]+)$");
			std::smatch match;
			std::string cmdStr(cmd);

			if (std::regex_match(cmdStr, match, pattern)) {
				int row = std::stoi(match[1].str());
				int col = std::stoi(match[2].str());
				int index = (row-1) + ((col-1) * COLS);

				if (row >= 1 && row < ROWS && col >= 1 && col < COLS) {
					BOARD[index] = static_cast<char>(State::Alive);
				}
				else {
					#if LINUX
						printf("%s", RED);
					#elif WIN
						setColour(static_cast<int>(Colour::WIN_RED));
					#endif
	
					printf("E: Coordinates out of bounds.\n");

					#if LINUX
						printf("%s", GREEN);
					#elif WIN
						setColour(static_cast<int>(Colour::WIN_GREEN));
					#endif
				}
			}
		}
		else if (cmd[0] == '-') {
			std::regex pattern("^\\-([0-9]+),([0-9]+)$");
			std::smatch match;
			std::string cmdStr(cmd);
			if (std::regex_match(cmdStr, match, pattern)) {
				int row = std::stoi(match[1].str());
				int col = std::stoi(match[2].str());
				int index = (row-1) + ((col-1) * COLS);

				if (row >= 1 && row < ROWS && col >= 1 && col < COLS) {
					BOARD[index] = static_cast<char>(State::Dead);
				}
				else {
					#if LINUX
						printf("%s", RED);
					#elif WIN
						setColour(static_cast<int>(Colour::WIN_RED));
					#endif
	
					printf("E: Coordinates out of bounds.\n");

					#if LINUX
						printf("%s", GREEN);
					#elif WIN
						setColour(static_cast<int>(Colour::WIN_GREEN));
					#endif
				}
			}
		}
		else {
			#if LINUX
				printf("%s", RED);
			#elif WIN
				setColour(static_cast<int>(Colour::WIN_RED));
			#endif
	
			printf("E: Invalid command.\n");

			#if LINUX
				printf("%s", GREEN);
			#elif WIN
				setColour(static_cast<int>(Colour::WIN_GREEN));
			#endif
		}
	}
};

// Main code

int main (void) {
	clear();


	char BOARD[BOARD_SIZE + 1];
	memset(BOARD, static_cast<char>(State::Dead), BOARD_SIZE); // Set the BOARD to be just dots (dead cell)
	BOARD[BOARD_SIZE] = '\0';

	#if LINUX	
		printf("%s", GREEN);
	#elif WIN
		setColour(static_cast<int>(Colour::WIN_GREEN));
	#endif

	printf("\t\t\t\tCONWAYS GAME OF LIFE v2.1.2\n");
	printf("\t\t\t\t\t- https://github.com/TierTheTora\n");
	#if LINUX
		printf("\t\tType \'");
		printx(true, false, false, false, "help");
		printf("\' for help.\n\n\n");
	#elif WIN
		printf("\t\tType \'help\' for help.\n\n\n");
	#endif

	while (true) { // Console prompt loop
		std::string cmd;
		printf("> ");
		std::getline(std::cin, cmd);

		Command::command(cmd, BOARD);
		//displayBoard(BOARD);
	}

	
	#if LINUX
		printf("%s", RESET);
	#elif WIN
		setColour(static_cast<int>(Colour::WIN_RESET));
	#endif
}

void update (char * BOARD) {
	clear();
	char newBoard[BOARD_SIZE + 1]; // Create the next board generation
	newBoard[BOARD_SIZE] = '\0';

	while (true) {
		for (int i = 0; i < BOARD_SIZE; ++i) { // Loop through each cell
			int neighbours = countNeighbours(BOARD, i); // Get the neighbouring cells of the current cell
			if (BOARD[i] == static_cast<char>(State::Alive)) { // If the cell is alive
				newBoard[i] = (neighbours == 2 || neighbours == 3) ? static_cast<char>(State::Alive) : static_cast<char>(State::Dead);  // If there are 2 or 3 neighbouring cellswe set the current cell to be alive
			} 
			else { // If the current cell is dead 
				newBoard[i] = (neighbours == 3) ? static_cast<char>(State::Alive) : static_cast<char>(State::Dead); // If there are 3 neighbouring cells we set the current cell to be alive
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
				if (BOARD[r * COLS + c] == static_cast<char>(State::Alive)) { // If the cell is alive increase the neighbour count
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
			if (BOARD[j] == static_cast<char>(State::Alive)) printf("%s", YELLOW);
		#elif WIN
			setColour(static_cast<int>(Colour::WIN_GREY));
			if (BOARD[j] == static_cast<char>(State::Alive)) setColour(static_cast<int>(Colour::WIN_YELLOW));
		#endif
		
		printf("%c ", BOARD[j]);
		
		#if LINUX
			printf("%s", GREY);
		#elif WIN
			setColour(static_cast<int>(Colour::WIN_GREY));
		#endif
		if ((j + 1) % ROWS == 0) { // If we are at the end of the first row
			printf("\n");
		}
	}
	printf("\n");
	#if LINUX
		printf("%s", GREEN);
	#elif WIN 
		setColour(static_cast<int>(Colour::WIN_GREEN));
	#endif
}
