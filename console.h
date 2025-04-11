#include <stdlib.h>
#include <string.h>

#define WIN   defined(_WIN32) || defined(_WIN64)
#define LINUX defined(__unix__) || defined(__linux__) || defined(__APPLE__) || defined(__MACH__)

#if WIN
	#include <windows.h>
	#include <conio.h>
#endif
#include <unistd.h>
#if LINUX
	#include <termios.h>
	#include <sys/select.h>
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

void clear() {

#if WIN
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = { 0, 0 };
	
	if (hConsole == INVALID_HANDLE_VALUE) return;
	
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;
	
	FillConsoleOutputCharacter(hConsole, ' ', cellCount, homeCoords, &count);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
	SetConsoleCursorPosition(hConsole, homeCoords);

#elif LINUX
	printf("\033[2J\033[H"); 
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
	termios orig_termios;
	
	void disableBufferedInput() {
		tcgetattr(STDIN_FILENO, &orig_termios); // Get current settings
		termios new_termios = orig_termios;
		
		new_termios.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
		tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
	}
	
	void restoreInput() {
		tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios); // Restore settings
	}
	
	bool keyPressed() {
		timeval tv = {0L, 0L}; // No waiting
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
	}
	
	char readChar() {
		char c;
		if (read(STDIN_FILENO, &c, 1) > 0) return c;
		return 0;
	}
#elif WIN
	void showCursor() {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(hConsole, &cursorInfo);
		cursorInfo.bVisible = TRUE;
		SetConsoleCursorInfo(hConsole, &cursorInfo);
	}
	void hideCursor() {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(hConsole, &cursorInfo);
		cursorInfo.bVisible = FALSE;
		SetConsoleCursorInfo(hConsole, &cursorInfo);
	}
	void terminate(DWORD fdwCtrlType) {
    		if (fdwCtrlType == CTRL_C_EVENT) {
			showCursor();
			setColour(static_cast<int>(Colour::WIN_RESET));
			exit(0);
		}
	}
	void disableBufferedInput() {
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode;
		
		// Get the current console mode
		GetConsoleMode(hStdin, &mode);
		
		// Disable line input and echo input
		mode &= ~ENABLE_LINE_INPUT;
		mode &= ~ENABLE_ECHO_INPUT;
		
		// Set the console mode with the changes
		SetConsoleMode(hStdin, mode);
	}
	
	void restoreInput() {
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode;
		
		// Get the current console mode
		GetConsoleMode(hStdin, &mode);
		
		// Enable line input and echo input again
		mode |= ENABLE_LINE_INPUT;
		mode |= ENABLE_ECHO_INPUT;
		
		// Set the console mode with the changes
		SetConsoleMode(hStdin, mode);
	}
	bool keyPressed() {
		return _kbhit();
	}
	
	char readChar() {
		return _getch();
	}

#endif

ssize_t printc (char arg) {
	return write (1, &arg, 1);
}
ssize_t print (const char* arg) {
	size_t size = strlen (arg);
	return write (1, arg, size);
}
ssize_t printd (const int arg) {
	char str[64];
	sprintf(str, "%d", arg); 
	size_t size = strlen (str);
	return write (1, &str, size);
}
