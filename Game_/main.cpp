#include "stdafx.h"
#include "MyConsole.h"
#include "game.h"


int main()
{
	initConsole();
	InitGame();

	while (bLoop)
	{
		KeyInput();
		gameLogic();//Update

		if (Skip() == false)
			Render();
	}

}