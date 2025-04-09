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
#include <fstream>
#include <stdbool.h>
#include <csignal>
#include <unistd.h>
#if WIN
	#include <windows.h>
#endif
// Globals

uint8_t            ROWS       = 20;
uint8_t            COLS       = 20;
int                BOARD_SIZE = ROWS * COLS;
int                SPEED      = 500;
unsigned long long GEN        = 0;
int                ALIVE      = 0;
bool               WRAP       = true;
char               VER[]      = "v2.5.0";

// Functions

int  countNeighbours (const char* BOARD, int i);
void update          (char * BOARD);
void displayBoard    (char * BOARD);
void step            (char * BOARD, int i);
int  err             (std::string e);
#if LINUX
void escape      (int signal);
#endif

// Classes

namespace Command {
	void command(std::string cmd, char ** BOARD) {
		// Convert cmd to be lowercase
		for (char& c : cmd) {
			c = std::tolower(c); 
		}
		std::string cmdo = cmd; // cmd with spaces
		// Trim spaces
		cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end()); // Removes the spaces
		if (cmd[0] == '#' || cmd == "") {

		}
		else if (cmd == "help") {
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
			printf(" 1.  +<int>,<int>      : Make a cell alive on the board \n \
2.  -<int>,<int>      : Make a cell dead on the board \n \
3.  board             : Show the current game board \n \
4.  run               : Run the simulation\n \
4.  load <file>       : Read a script file\n \
5.  speed=<int>       : Set the game speed (ms)\n \
6.  size=<int>,<int>  : Set board size (MAX: 255x255)\n \
7.  exit/quit         : Exit the program\n \
8.  stat              : Show information on the board\n \
9.  step <int>        : Step forward a given amount of generations\n \
10. clear             : Clear the board\n \
11. fill              : Fill the board with Alive cells\n \
12. wrap=<on/off>     : Turn board wrapping on or off\n\n");
		}
		else if (cmdo[0] == 'l') {
			std::regex pattern("^load\\s+(.*)$");
			std::smatch match;

			if (std::regex_match(cmdo, match, pattern)) {
				std::string file = match[1].str();
				std::string word;
			
				std::ifstream read(file);
				if (read) {
					while (std::getline(read, word)) {
						Command::command(word, BOARD);
					}
					read.close();
				}
				else {
					err("File does not exist");
				}
			}
			else err("Invalid command");
		}
		else if (cmd == "board") {
			displayBoard(*BOARD);
		}
		else if (cmd == "run") {
			update(*BOARD);
		}
		else if (cmd == "clear") {
			memset(*BOARD, static_cast<char>(State::Dead), BOARD_SIZE);
		}
		else if (cmd == "exit" || cmd == "quit") {
			#if LINUX
				printf("%s", RESET);
			#elif WIN
				setColour(static_cast<int>(Colour::WIN_RESET));
			#endif
			delete[] *BOARD;
			exit(0);
		}
		else if (cmd[0] == 'w') {
			std::regex pattern("^wrap=+(.*)$");
			std::smatch match;

			if (std::regex_match(cmd, match, pattern)) {
				std::string opt = match[1].str();
				WRAP = opt == "on";
				if (opt != "on" && opt != "off") {
					err("Wrap must be on or off");
				}
			}
			else {
				err("Invalid command");
			}
		}
		else if (cmd[0] == 's') { 
			std::regex pattern("^speed=([0-9]+)$");
			std::regex ptrn("^size=([0-9]+),([0-9]+)$");
			std::regex ptrn2("^step([0-9]+)$");
			std::smatch match;
			if (cmd == "stat") {
				ALIVE = 0;
				for (int i = 0; i < BOARD_SIZE; ++i) { // Iterate thrugh board
					if ((*BOARD)[i] == static_cast<char>(State::Alive)) { // If cell is alive
						++ALIVE;
					}
				}
				int dead = BOARD_SIZE - ALIVE;
				printf("Generation:  %Ld\n", GEN);
				printf("Alive cells: %d\n", ALIVE);
				printf("Dead cells:  %d\n", dead);

			}
			else if (std::regex_match(cmd, match, pattern)) {
				SPEED = std::stoi(match[1].str());
			}
			else if (regex_match(cmd, match, ptrn)) {
				ROWS = std::stoi(match[1].str());
				COLS = std::stoi(match[2].str());
				BOARD_SIZE = ROWS * COLS;
				
				delete[] *BOARD; // Delete old board
				
				*BOARD = new char[BOARD_SIZE + 1];
				memset(*BOARD, static_cast<char>(State::Dead), BOARD_SIZE);
				(*BOARD)[BOARD_SIZE] = '\0';
			}
			else if (regex_match(cmd, match, ptrn2)) {
				int i = std::stoi(match[1].str());
				step(*BOARD, i);
			}
			else {
				err("Invalid command");
			}
		}
		else if (cmd[0] == 'f') {
			std::regex pattern("^fill([0-9]+),([0-9]+),([0-9]+),([0-9]+)$");
			std::smatch match;
			if (std::regex_match(cmd, match, pattern)) {
				int x = std::stoi(match[1].str());
				int y = std::stoi(match[2].str());
				int z = std::stoi(match[3].str());
				int w = std::stoi(match[4].str());

				if (x > z || y > w) {
					err("Fill coordinates must be top-left to bottom-right");
				}


				int left   = std::min(x, z);
				int right  = std::max(x, z);
				int top    = std::min(y, w);
				int bottom = std::max(y, w);

				for (int r = top; r <= bottom; ++r) {
					for (int c = left; c <= right; ++c) {
						if (r >= 1 && r <= ROWS && c >= 1 && c <= COLS) {
							int i = (r - 1) * COLS + (c - 1);
							(*BOARD)[i] = static_cast<char>(State::Alive);
						}
					}
				}
			
			}
			else if (cmd == "fill") {
				delete[] *BOARD;
				*BOARD = new char[BOARD_SIZE + 1];
				memset(*BOARD, static_cast<char>(State::Alive), BOARD_SIZE);
				(*BOARD)[BOARD_SIZE] = '\0';
			}
			else err("Invalid command");
		}
		else if (cmd[0] == '+') {
			std::regex pattern("^\\+([0-9]+),([0-9]+)$");
			std::smatch match;

			if (std::regex_match(cmd, match, pattern)) {
				int row = std::stoi(match[1].str());
				int col = std::stoi(match[2].str());
				int index = (row-1) + ((col-1) * COLS);

				if (row >= 1 && row < ROWS && col >= 1 && col < COLS) {
					(*BOARD)[index] = static_cast<char>(State::Alive);
				}
				else {
					err("Coordinates out of bounds");
				}
			}
			else err("Invalid command");
		}
		else if (cmd[0] == '-') {
			std::regex pattern("^\\-([0-9]+),([0-9]+)$");
			std::smatch match;
			if (std::regex_match(cmd, match, pattern)) {
				int row = std::stoi(match[1].str());
				int col = std::stoi(match[2].str());
				int index = (row-1) + ((col-1) * COLS);

				if (row >= 1 && row < ROWS && col >= 1 && col < COLS) {
					(*BOARD)[index] = static_cast<char>(State::Dead);
				}
				else {
					err("Coordinates out of bounds");
				}
			}
			else err("Invalid command");
		}
		else {
			err("Invalid command");
		}
	}
};



// Main code

int main (void) {
	clear();
	#if WIN
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)terminate, TRUE);
	#endif

	char* BOARD = new char[BOARD_SIZE + 1];
	memset(BOARD, static_cast<char>(State::Dead), BOARD_SIZE);
	BOARD[BOARD_SIZE] = '\0';

	#if LINUX	
		printf("%s", GREEN);
	#elif WIN
		setColour(static_cast<int>(Colour::WIN_GREEN));
	#endif

	printf("\t\t\t\tCONWAYS GAME OF LIFE %s\n", VER);
	printf("\t\t\t\t\t- https://github.com/TierTheTora\n");
	#if LINUX
		printf("\t\tType \'");
		printx(true, false, false, false, "help");
		printf("\' for help.\n\n\n");
	#elif WIN
		printf("\t\tType \'help\' for help.\n\n\n");
	#endif

	#if LINUX
		std::signal(SIGINT, escape); // If program is terminated (^C)
	#endif

	while (true) { // Console prompt loop
		std::string cmd;
		printf("> ");
		std::getline(std::cin, cmd);

		Command::command(cmd, &BOARD);
		//displayBoard(BOARD);
	}

	
	#if LINUX
		printf("%s", RESET);
	#elif WIN
		setColour(static_cast<int>(Colour::WIN_RESET));
	#endif
	delete[] BOARD;
}

void update (char * BOARD) {
	#if LINUX
		disableBufferedInput(); // Turn off line buffering
		atexit(restoreInput);   // Ensure input settings are restored on exit
	#endif
	clear();
	#if LINUX
		// Hide cursor
		printf("\033[?25l"); 
	#endif
	char newBoard[BOARD_SIZE]; // Create the next board generation

	while (true) {
		// Break if Q or X 
		if (keyPressed()) {
			if (keyPressed()) {
				char c = readChar();
				if (c == 'q' || c == 'x') {
						restoreInput();	
						#if LINUX
							printf("\033[?25h"); // Show cursor
						#elif WIN
							showCursor();
						#endif
					break;
				}
			}
		}
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
		#if LINUX
			// Hide cursor
			printf("\033[?25l");
		#elif WIN
			hideCursor();
		#endif
		++GEN;
		printf("Generation: %Ld\n", GEN);
		displayBoard(BOARD);
		std::this_thread::sleep_for(std::chrono::milliseconds(SPEED)); // Wait 1 second
		#if LINUX
			// Show cursor
			printf("\033[?25h");
		#endif
	}
}

void step (char * BOARD, int i) {
	char newBoard[BOARD_SIZE]; // Create the next board generation

	 for (int j = 0; j < i; ++j) {
		for (int i = 0; i < BOARD_SIZE; ++i) { // Loop through each cell
			int neighbours = countNeighbours(BOARD, i); // Get the neighbouring cells of the current cell
			if (BOARD[i] == static_cast<char>(State::Alive)) { // If the cell is alive
				newBoard[i] = (neighbours == 2 || neighbours == 3) ? static_cast<char>(State::Alive) : static_cast<char>(State::Dead);  // If there are 2 or 3 neighbouring cellswe set the current cell to be alive
			} 
			else { // If the current cell is dead 
				newBoard[i] = (neighbours == 3) ? static_cast<char>(State::Alive) : static_cast<char>(State::Dead); // If there are 3 neighbouring cells we set the current cell to be alive
			}
		}
		GEN++;
		memcpy(BOARD, newBoard, BOARD_SIZE); // Set the board to the next generation
	}
}

int countNeighbours (const char * BOARD, int i) {
	int row = i / COLS; // Get the current row
	int col = i % COLS; // Get the current column
	int count = 0; // Neighbouring cell count

	for (int dr = -1; dr <= 1; ++dr) { // Check surrounding rows
		for (int dc = -1; dc <= 1; ++dc) { // Check surrounding columns
			if (dr == 0 && dc == 0) continue; // Skip the current cell
			if (!WRAP) {
				int r = row + dr; // Row the neighour is at
				int c = col + dc; // Column the neighbour is at
				if (r >= 0 && r < ROWS && c >= 0 && c < COLS) { // Check if the neighbour is inside the board
					if (BOARD[r * COLS + c] == static_cast<char>(State::Alive)) { // If the cell is alive increase the neighbour count
						++count;
					}
				}
			}
			else {
				int r = (row + dr + ROWS) % ROWS;
				int c = (col + dc + COLS) % COLS;

				if (BOARD[r * COLS + c] == static_cast<char>(State::Alive)) {
					++count;
				}
			}
		}
	}
	return count;
}

void displayBoard (char * BOARD) {
	printf("\n");
	for (int j = 0; j < BOARD_SIZE; ++j) { // Iterate through each cell
		int rsp = std::to_string(j).length();
		int csp = std::to_string(j).length();

		if (!j) { // If j == 0
			printf("  ");
			for (int k = 1; k <= COLS; ++k) {
				printf("%d ", k % 10);
			}
			printf("\n");
		}
		if (j % COLS == 0) {
			#if LINUX
				printf("%s", GREEN);
			#elif WIN
				setColour(static_cast<int>(Colour::WIN_GREEN));
			#endif
			printf("%d ", (j / COLS + 1) % 10);
			#if LINUX
				printf("%s", GREY);
			#elif WIN
				setColour(static_cast<int>(Colour::WIN_GREY));
			#endif
		}
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
		if ((j + 1) % COLS == 0) { // If we are at the end of the first row
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

int err (std::string e) {
	#if LINUX
		printf("%s", RED);
	#elif WIN
		setColour(static_cast<int>(Colour::WIN_RED));
	#endif
		
	std::cout << "E: "<< e << ".\n";

	#if LINUX
		printf("%s", GREEN);
	#elif WIN
		setColour(static_cast<int>(Colour::WIN_GREEN));
	#endif
	return 1;
}
#if LINUX
	void escape (int signal) {
		std::cout << "\033[?25h"; // Show cursor
		std::cout.flush(); // Make sure it prints immediately
		std::exit(signal); // Exit with the received signal
	}
#endif
