/*
 * Created by github.com/TierTheTora
 * Lisence MIT
 * Version 2.7.1
 * Repository github.com/TierTheTora/game-of-life
 */


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
#include <time.h>
#include "presets.h"
#include <unordered_map>
#if WIN
	#include <windows.h>
#endif
// Globals

char helps[][1024] = {
   "help (cmd) : Show the help menu | Show the help of command (int page)\n",
   "+<int>,<int> : Place a coordinate on the board at x,y (Col, Row)\n",
   "-<int>,<int> : Remove a coordinate on the board at x,y (Col, Row)\n",
   "board : Display the current board (No args)\n",
   "run : Run the current board state (Press x or q to quit)\n",
   "load <file> : Load a script file to be executed by the interpreter\n",
   "speed=<int> : Set the amount of time it takes for a generation to complete (ms)\n",
   "size=<int>,<int> : Clear and set the size of the the board (x,y)\n",
   "exit/quit : Quit the program with exit code 0\n",
   "stat : Get statistics on board like the dimensions and cells\n",
   "step <int> : Advance a given amount of generations forward\n",
   "clear (x,y,x,y) : Set the board to be filled with dead cells | Fill a rectangle with dead cells\n",
   "fill (x,y,x,y) : Set the board to be filled with alive cells | Fill a rectangle with alive cells\n",
   "wrap=<on/off> : Allow or disallow cells to wrap around the board (may cause issues)\n",
   "save <file> : Save the current board state as a script file\n",
   "restore : Get the board state back from before the simulation ran\n",
   "random <int> : Set the board to random cells | int: set the dead space (big int = lots of dead area)\n",
   "pattern (name) : Load a predefined pattern onto the board | Get the a list patterns\n",
   "mirror <pos> : Mirror the current board (v | h | vertical | horizontal)\n",
   "invert : Invert each cell of the board: dead -> alive, alive -> dead\n"
};
uint8_t            ROWS       = 20;
uint8_t            COLS       = 20;
uint8_t            OLD_ROWS   = 20;
uint8_t            OLD_COLS   = 20;
int                BOARD_SIZE = ROWS * COLS;
int                OLD_SIZE   = ROWS * COLS;
int                SPEED      = 500;
unsigned long long GEN        = 0;
int                ALIVE      = 0;
bool               WRAP       = false;
char               VER[]      = "v2.7.1";

// Functions

int  countNeighbours (const char* BOARD, int i);
void update          (char * BOARD);
void displayBoard    (char * BOARD);
void step            (char * BOARD, int i);
int  err             (std::string e);
int  page            (int i);
#if LINUX
void escape      (int signal);
#endif

// Classes
struct boardReturnStruct {
	char *BOARD;
	char *BOARD_OLD;
} BOARD_RETURN_STRUCT;

namespace Command {
	 void command(std::string cmd, char * BOARD, char * BOARD_OLD) {
		// Convert cmd to be lowercase
		for (char& c : cmd) {
			c = tolower(c); 
		}
		std::string cmdo = cmd; // cmd with spaces
		// Trim spaces
		cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end()); // Removes the spaces
		if (cmd[0] != '#' && cmd != "") {
			if (cmd[0] == 'h') {
            std::regex regex ("^help\\s*(.+)$");
            std::smatch match;
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
			   	printf(" 1.  help <cmd>        : Get the full description of a command\n \
2.  +<int>,<int>      : Make a cell alive on the board \n \
3.  -<int>,<int>      : Make a cell dead on the board \n \
4.  board             : Show the current game board \n \
5.  run               : Run the simulation\n \
6.  load <file>       : Read a script file\n \
7.  speed=<int>       : Set the game speed (ms)\n \
8.  size=<int>,<int>  : Set board size (MAX: 255x255)\n \
9.  exit/quit         : Exit the program\n \
10. stat              : Show information on the board\n \
11. step <int>        : Step forward a given amount of generations\n \
12. clear             : Clear the board\n \
13. fill              : Fill the board with Alive cells\n \
14. wrap=<on/off>     : Turn board wrapping on or off\n \
15. save <file>       : Save the current board as a file\n \
16. restore           : Restore the board from before the run command\n \
17. random <int>      : Get a random board\n \
18. pattern <name>    : Load a pattern\n \
19. mirror <pos>      : Mirror the current board\n \
20. invert            : Invert each cell of the board\n\n");
            }
            else if (std::regex_match(cmdo, match, regex)) {
               std::unordered_map<std::string, int> KEY = {
                  {"help", 0},
                  {"+", 1},
                  {"-", 2},
                  {"board", 3},
                  {"run", 4},
                  {"load", 5},
                  {"speed", 6},
                  {"size", 7},
                  {"exit", 8},
                  {"quit", 8},
                  {"stat", 9},
                  {"step", 10},
                  {"clear", 11},
                  {"fill", 12},
                  {"wrap", 13},
                  {"save", 14},
                  {"restore", 15},
                  {"random", 16},
                  {"pattern", 17},
                  {"mirror", 18},
                  {"invert", 19}
               };
               std::string helpCmd = match[1].str();
               auto it = KEY.find(helpCmd);
               if (it != KEY.end()) {
                  int index = it->second;
                  page(index);
               }
               else err("Ivalid command");
            }
            else err("Ivalid command");
         }
         else if (cmd[0] == 'm') {
            std::regex pattern("^mirror\\s*(h|v|horizontal|vertical)$");
            std::smatch match;
            if (std::regex_match(cmdo, match, pattern)) {
               std::string axis = match[1].str();
               if (axis == "h" || axis == "horizontal") {
                  for (int r = 0; r < ROWS / 2; ++r) {
                     for (int c = 0; c < COLS; ++c) {
                        int top = r * COLS + c;
                        int bottom = (ROWS - 1 - r) * COLS + c;
                        std::swap(BOARD[top], BOARD[bottom]);
                     }
                  }
               }
               int index = std::stoi(match[1].str());
               page(index-1);
            }
			}
         else if (cmd[0] == 'm') {
            std::regex pattern("^mirror\\s*(h|v|horizontal|vertical)$");
            std::smatch match;
            if (std::regex_match(cmdo, match, pattern)) {
               std::string axis = match[1].str();
               if (axis == "h" || axis == "horizontal") {
                  for (int r = 0; r < ROWS / 2; ++r) {
                     for (int c = 0; c < COLS; ++c) {
                        int top = r * COLS + c;
                        int bottom = (ROWS - 1 - r) * COLS + c;
                        std::swap(BOARD[top], BOARD[bottom]);
                     }
                  }
               }
               else if (axis == "v" || axis == "vertical") {
                  for (int r = 0; r < ROWS; ++r) {
                     for (int c = 0; c < COLS / 2; ++c) {
                        int left = r * COLS + c;
                        int right = r * COLS + (COLS - 1 - c);
                        std::swap(BOARD[left], BOARD[right]);
                     }
                  }
               }
            }
            else err("Invalid command");
         }
         else if (cmd[0] == 'p') {
            std::unordered_map<std::string, unsigned char*> KEY = {
               {"gospergun", gospergun},
               {"glider", glider},
               {"toad", toad},
               {"blinker", blinker}
            };
            std::regex regex ("^pattern\\s+(.+)$");
            std::smatch match;
            if (std::regex_match(cmdo, match, regex)) {
               std::string ptrn = match[1].str();
               auto it = KEY.find(ptrn);
               if (it != KEY.end()) {
                  unsigned char * pattern = it->second;
                  std::string str (reinterpret_cast<char*>(pattern));
                  std::istringstream stream(str);
                  std::string line;
                  
                  delete[] BOARD;
                  delete[] BOARD_OLD;
                  BOARD = new char[BOARD_SIZE + 1];
                  BOARD_OLD = new char[BOARD_SIZE + 1];
                  BOARD[BOARD_SIZE] = '\0';
                  BOARD_OLD[BOARD_SIZE] = '\0';

                  while (std::getline(stream, line)) {
                     Command::command(line, BOARD, BOARD_OLD);
               		BOARD = BOARD_RETURN_STRUCT.BOARD;
               		BOARD_OLD = BOARD_RETURN_STRUCT.BOARD_OLD;
                  }
               }
            }
            else if (cmd == "pattern") {
               for (auto it = KEY.begin(); it != KEY.end(); ++it) {
                  std::cout << it->first << '\n';
               }
            }
            else err("Invalid command");
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
							Command::command(word, BOARD, BOARD_OLD);
               		BOARD = BOARD_RETURN_STRUCT.BOARD;
               		BOARD_OLD = BOARD_RETURN_STRUCT.BOARD_OLD;
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
				displayBoard(BOARD);
				fflush(stdout);
			}
			else if (cmd[0] == 'r') {
            std::regex regex ("^random\\s*([0-9]+)$");
            std::smatch match;
				if (cmd == "run") {
					OLD_ROWS = ROWS;
					OLD_COLS = COLS;
					OLD_SIZE = ROWS * COLS;
					delete[] BOARD_OLD;
					BOARD_OLD = new char[OLD_SIZE + 1];
					BOARD_OLD[OLD_SIZE] = '\0';
					memcpy(BOARD_OLD, BOARD, OLD_SIZE * sizeof(char));
					update(BOARD);
				}
				else if (cmd == "restore") {
					delete[] BOARD;
					BOARD = new char[OLD_SIZE + 1];
					BOARD[OLD_SIZE] = '\0';
					BOARD_SIZE = OLD_SIZE;
					memcpy(BOARD, BOARD_OLD, OLD_SIZE * sizeof(char));
					COLS = OLD_COLS;
					ROWS = OLD_ROWS;
				}
            else if (std::regex_match(cmdo, match, regex)) {
               int chance = std::stoi(match[1].str());
               srand(time(NULL));
               for (int i = 0; i < BOARD_SIZE; ++i) {
                  int cell = rand() % chance;
                  if (cell == 1) {
                     BOARD[i] = '#';
                  }
                  else {
                     BOARD[i] = '.';
                  }
               }
            }
				else err("Invalid command");
			}
			else if (cmd[0] == 'c') {
				std::regex pattern("^clear([0-9]+),([0-9]+),([0-9]+),([0-9]+)$");
				std::smatch match;
				if (std::regex_match(cmd, match, pattern)) {
					int x = std::stoi(match[1].str());
					int y = std::stoi(match[2].str());
					int z = std::stoi(match[3].str());
					int w = std::stoi(match[4].str());
	
					if (x > z || y > w) {
						err("Clear coordinates must be top-left to bottom-right");
					}
	
	
					int left   = std::min(x, z);
					int right  = std::max(x, z);
					int top    = std::min(y, w);
					int bottom = std::max(y, w);
	
					for (int r = top; r <= bottom; ++r) {
						for (int c = left; c <= right; ++c) {
							if (r >= 1 && r <= ROWS && c >= 1 && c <= COLS) {
								int i = (r - 1) * COLS + (c - 1);
								(BOARD)[i] = '.';
							}
						}
					}
				}
				else if (cmd == "clear") {
               GEN = 0;
					memset(BOARD, '.', BOARD_SIZE);
				}
				else err("Invalid command");
			}
			else if (cmd == "exit" || cmd == "quit") {
				#if LINUX
					printf("%s", RESET);
				#elif WIN
					setColour(static_cast<int>(Colour::WIN_RESET));
				#endif
				delete[] BOARD;
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
				std::regex pattern ("^speed=([0-9]+)$");
				std::regex ptrn    ("^size=([0-9]+),([0-9]+)$");
				std::regex ptrn2   ("^step([0-9]+)$");
				std::regex ptrn3   ("^save\\s+(.+)$");
				std::smatch match;
				if (cmd == "stat") {
					ALIVE = 0;
					for (int i = 0; i < BOARD_SIZE; ++i) { // Iterate thrugh board
						if ((BOARD)[i] == '#') { // If cell is alive
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
					
					delete[] BOARD; // Delete old board
	
					BOARD = new char[BOARD_SIZE + 1];
					memset(BOARD, '.', BOARD_SIZE);
					(BOARD)[BOARD_SIZE] = '\0';
				}
				else if (regex_match(cmd, match, ptrn2)) {
					int i = std::stoi(match[1].str());
					step(BOARD, i);
				}
				else if (regex_match(cmdo, match, ptrn3)) {
					std::string fname = match[1].str();
					std::ofstream file (fname);
					file << "size=" << (int)ROWS << "," << (int)COLS << "\n";
					for (int i = 0; i < BOARD_SIZE; ++i) {
						if ((BOARD)[i] == '#') {
							int row = i / COLS;
							int col = i % COLS;
							file << "+" << col+1 << "," << row+1 << "\n";
						}
					}
					file.close();
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
								(BOARD)[i] = '#';
							}
						}
					}
				
				}
				else if (cmd == "fill") {
					delete[] BOARD;
					BOARD = new char[BOARD_SIZE + 1];
					memset(BOARD, '#', BOARD_SIZE);
					(BOARD)[BOARD_SIZE] = '\0';
				}
				else err("Invalid command");
			}
         else if (cmd == "invert") {
            for (int i = 0; i < BOARD_SIZE; ++i) {
               if (BOARD[i] == '#') {
                  BOARD[i] = '.';
               }
               else {
                  BOARD[i] = '#';
               }
            }
         }
			else if (cmd[0] == '+') {
				std::regex pattern("^\\+([0-9]+),([0-9]+)$");
				std::smatch match;
	
				if (std::regex_match(cmd, match, pattern)) {
					int row = std::stoi(match[1].str());
					int col = std::stoi(match[2].str());
					int index = (row-1) + ((col-1) * COLS);
	
					if (row >= 1 && row <= ROWS && col >= 1 && col <= COLS) {
						(BOARD)[index] = '#';
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
	
					if (row >= 1 && row <= ROWS && col >= 1 && col <= COLS) {
						(BOARD)[index] = '.';
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
		BOARD_RETURN_STRUCT.BOARD = BOARD;
		BOARD_RETURN_STRUCT.BOARD_OLD = BOARD_OLD;
	}
};



// Main code

int main (void) {
	clear();
	#if WIN
		SetConsoleCtrlHandler((PHANDLER_ROUTINE)terminate, TRUE);
	#endif

	char* BOARD = new char[BOARD_SIZE + 1];
	memset(BOARD, '.', BOARD_SIZE);
	BOARD[BOARD_SIZE] = '\0';
	
	char* BOARD_OLD = new char[BOARD_SIZE + 1];
	memset(BOARD_OLD, '.', BOARD_SIZE);
	BOARD_OLD[BOARD_SIZE] = '\0';
	
	std::string word;
	std::ifstream file("conway.config");
	if (file) {
		while (std::getline(file, word)) {
			Command::command(word, BOARD, BOARD_OLD);
		}
		file.close();
	}

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

		Command::command(cmd, BOARD, BOARD_OLD);
		BOARD = BOARD_RETURN_STRUCT.BOARD;
		BOARD_OLD = BOARD_RETURN_STRUCT.BOARD_OLD;
		//displayBoard(BOARD);
	}

	
	#if LINUX
		printf("%s", RESET);
	#elif WIN
		setColour(static_cast<int>(Colour::WIN_RESET));
	#endif
	delete[] BOARD;
	delete[] BOARD_OLD;
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
	char* newBoard = new char[BOARD_SIZE]; // Create the next board generation

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
		clear();
		for (int i = 0; i < BOARD_SIZE; ++i) { // Loop through each cell
			int neighbours = countNeighbours(BOARD, i); // Get the neighbouring cells of the current cell
			if (BOARD[i] == '#') { // If the cell is alive
				newBoard[i] = (neighbours == 2 || neighbours == 3) ? '#' : '.';  // If there are 2 or 3 neighbouring cells we set the current cell to be alive
			} 
			else { // If the current cell is dead 
				newBoard[i] = (neighbours == 3) ? '#' : '.'; // If there are 3 neighbouring cells we set the current cell to be alive
			}
		}
		memcpy(BOARD, newBoard, BOARD_SIZE); // Set the board to the next generation
		#if LINUX
			// Hide cursor
			printf("\033[?25l");
		#elif WIN
			hideCursor();
		#endif
		++GEN;
		printf("Generation: %Ld\n", GEN);
		displayBoard(BOARD);
		fflush(stdout);
		std::this_thread::sleep_for(std::chrono::milliseconds(SPEED)); // Wait 1 second
		#if LINUX
			// Show cursor
			printf("\033[?25h");
		#endif
	}
	delete[] newBoard;
}

void step (char * BOARD, int i) {
	char* newBoard = new char[BOARD_SIZE]; // Create the next board generation

	for (int j = 0; j < i; ++j) {
		for (int i = 0; i < BOARD_SIZE; ++i) { // Loop through each cell
			int neighbours = countNeighbours(BOARD, i); // Get the neighbouring cells of the current cell
			if (BOARD[i] == '#') { // If the cell is alive
				newBoard[i] = (neighbours == 2 || neighbours == 3) ? '#' : '.';  // If there are 2 or 3 neighbouring cellswe set the current cell to be alive
			} 
			else { // If the current cell is dead 
				newBoard[i] = (neighbours == 3) ? '#' : '.'; // If there are 3 neighbouring cells we set the current cell to be alive
			}
		}
		GEN++;
		memcpy(BOARD, newBoard, BOARD_SIZE); // Set the board to the next generation
	}
	delete[] newBoard;
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
					if (BOARD[r * COLS + c] == '#') { // If the cell is alive increase the neighbour count
						++count;
					}
				}
			}
			else {
				int r = (row + dr + ROWS) % ROWS;
				int c = (col + dc + COLS) % COLS;

				if (BOARD[r * COLS + c] == '#') {
					++count;
				}
			}
		}
	}
	return count;
}

void displayBoard (char * BOARD) {
	printf("\n");
	bool gbo = GEN > 1;
	#if LINUX
		if (gbo) printf("\033[%dA", COLS); // Ansi escape for lines up
	#endif
	for (int j = 0; j < BOARD_SIZE; ++j) { // Iterate through each cell
		int rsp = std::to_string(j).length();
		int csp = std::to_string(j).length();

		if (!j) { // If j == 0
			print("  ");
			for (int k = 1; k <= COLS; ++k) {
				printd(k % 10);
				print(" ");
			}
			printc('\n');
		}
		if (j % COLS == 0) {
			#if LINUX
				print(GREEN);
			#elif WIN
				setColour(static_cast<int>(Colour::WIN_GREEN));
			#endif
			printd((j / COLS + 1) % 10);
			printc(' ');
			#if LINUX
				print(GREY);
			#elif WIN
				setColour(static_cast<int>(Colour::WIN_GREY));
			#endif
		}
		#if LINUX
			print(GREY);
			if (BOARD[j] == '#') print(YELLOW);
		#elif WIN
			setColour(static_cast<int>(Colour::WIN_GREY));
			if (BOARD[j] == '#') setColour(static_cast<int>(Colour::WIN_YELLOW));
		#endif
		
		printc(BOARD[j]);
		printc(' ');
		
		#if LINUX
			print(GREY);
		#elif WIN
			setColour(static_cast<int>(Colour::WIN_GREY));
		#endif
		if (!((j + 1) % COLS)) { // If we are at the end of the first row
			printc('\n');
		}
	}
	printc('\n');
	#if LINUX
		print(GREEN);
	#elif WIN 
		setColour(static_cast<int>(Colour::WIN_GREEN));
	#endif
	#if LINUX
		if (gbo) printf("\033[%dB", COLS); // Lines down
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
		printf("\033[?25h"); // Show cursor
		printf("%s", RESET); // Reset colours
		std::cout.flush(); // Make sure it prints immediately
		std::exit(signal); // Exit with the received signal
	}
#endif

int page (int i) {
   if (i > sizeof(helps) / sizeof(helps[0])) {
      err("Index out of bounds");
      return 1;
   }
   printf("%s", helps[i]);
   return 0;
}
