#include <stdlib.h>
#include <string.h>

// FOREGROUND CONSOLE COLOURS

	const char * RED    = "\033[31m";
	const char * YELLOW = "\033[33m";
	const char * GREY   = "\033[30m";
	const char * GREEN  = "\033[32m";
	const char * RESET  = "\033[0m";

// BACKGROUND CONSOLE COLOURS

	const char * BGREY  = "\033[40m";
	const char * BRESET = "\033[49m";

// FUNCTIONS

int clear() {
	return system("clear");
}

void printx (const bool it, const bool un, const bool bd, const bool st, const char * TEXT) {
	if (it) printf("\e[3m");
	if (un) printf("\e[4m");
	if (bd) printf("\e[1m");
	if (st) printf("\e[9m");
	printf("%s\e[23m\e[22m\e[24m\e[29m", TEXT);
}
