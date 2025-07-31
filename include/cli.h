// cli.h: Terminal interface stuff
#pragma once

#include <windows.h>

void
set_color(int fg, int bg) {
	/*
	0 = Default bg color (black on most terminals)
	1 = Dark Blue
	2 = Dark Green
	3 = Dark Cyan
	4 = Dark Red
	5 = Dark Magenta
	6 = Dark Yellow
	7 = Default fg color (bright gray/mild white on most terminals)
	8 = Bright Black (dark shade of gray on most terminals)
	9 = Bright Blue
	10 = Bright Green
	11 = Bright Cyan
	12 = Bright Red
	13 = Bright Magenta
	14 = Bright Yellow
	15 = Bright White

	Values bigger than 15 will cause some wacky shit to happen
	*/

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bg << 4) | fg);
}
