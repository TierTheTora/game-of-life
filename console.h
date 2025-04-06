#include <stdlib.h>
#include <string.h>

#define WIN   defined(_WIN32) || defined(_WIN64)
#define LINUX defined(__unix__) || defined(__linux__) || defined(__APPLE__) || defined(__MACH__)

#if WIN
	#include <windows.h>
#endif

// FOREGROUND CONSOLE COLOURS
#if LINUX
	// Linux terminal ANSII codes
	const char * RED    = "\033[31m";
	const char * YELLOW = "\033[33m";
	const char * GREY   = "\033[30m";
	const char * GREEN  = "\033[32m";
	const char * RESET  = "\033[00m";
#elif WIN
	// Windows colour values
	enum class Colour : int {
		WIN_RED     = 4,
		WIN_YELLOW  = 14,
		WIN_GREY    = 8,
		WIN_GREEN   = 2,
		WIN_RESET   = 15
	};

	void setColour (int colour) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour); // Set colour on windwos
	}
#endif

// CELLS

enum class State : char {
	Alive = '#',
	Dead  = '.'
};

// FUNCTIONS

int clear() {

#if LINUX
	return system("clear");
#elif WIN
	return system("cls");
#endif

}

#if LINUX
	void printx (const bool it, const bool un, const bool bd, const bool st, const char * TEXT) {
		if (it) printf("\e[3m");
		if (un) printf("\e[4m");
		if (bd) printf("\e[1m");
		if (st) printf("\e[9m");
		printf("%s\e[23m\e[22m\e[24m\e[29m", TEXT);
	}
#endif
