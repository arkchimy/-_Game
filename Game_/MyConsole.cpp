#include "stdafx.h"
#include "MyConsole.h"

static HANDLE hConsole;

void initConsole()
{
	timeBeginPeriod(1);

	CONSOLE_CURSOR_INFO cs_info;
	cs_info.bVisible = false;
	cs_info.dwSize = 100;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &cs_info);

	system("mode con cols=100 lines=50 | title MyGame");

}

void cs_Move(int x, int y)
{
	//buffer Ãâ·Â

	COORD pos;
	pos.X = x;
	pos.Y = y;

	SetConsoleCursorPosition(hConsole, pos);
}

