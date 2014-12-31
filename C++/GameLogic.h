#pragma once
#include <iostream>
#include <stdio.h>

using namespace std;

/*
	Takes care of the game logic
	Checks if a player has won or not
	Checks for illegal move (i.e. 3 on 3, 6 in a row, existing move)
*/

class GameLogic{
	char (*board)[15][15];
	pair<bool,bool> legalWin(int x, int y, char c);
	bool onBoard(int x, int y);
public:
	GameLogic(){}//:moveIsIllegal(false){}
	~GameLogic(){}

	void setBoard(char (*b)[15][15]){ board = b; }
	bool isBoardFilled();
	bool hasWon(int x, int y, char c);
	bool isLegalMove(int x, int y, char c);
};

