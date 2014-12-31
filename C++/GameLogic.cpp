#include "GameLogic.h"

bool GameLogic::isLegalMove(int x, int y, char c){
	return legalWin(x,y,c).first;
}

bool GameLogic::hasWon(int x, int y, char c){
	return legalWin(x,y,c).second;
}

bool GameLogic::isBoardFilled(){
	for(int i = 0; i<15; i++){
		for(int j = 0; j<15; j++){
			if((*board)[i][j] == '+')
				return false;
		}
	}
	return true;
}


bool GameLogic::onBoard(int i, int j) {
	return (i >= 0 || j >= 0 || i <= 14 || j <= 14);
}

// first bool determines if move is legal, second is if it is a winning move
pair<bool,bool> GameLogic::legalWin(int i, int j, char c) {
	// if space is occupied our outside board boundary return false
	if(!onBoard(i,j) || (*board)[i][j] != '+') 
		return pair<bool,bool>(false,false);
	// check if the move violates three and three rule
	bool win = false;
	int openThree = 0; // open three rows
	char match = c;
	// these arrays store the four line directions through i, j
	int yincr[4] = { 0, -1, 1, -1 };
	int xincr[4] = { 1, 1, 1, 0 };
	int counter;     // how many pieces in current row, including at (i,j)
	bool open; // is current row blocked at end?

	for(int k = 0; k < 4; ++k) {
		counter = 1;
		open = true;
		int a = i+yincr[k], b = j+xincr[k];
		char curr = '+';
		if(onBoard(a,b)) curr = (*board)[a][b];
		while(onBoard(a,b) && curr == match) {
			++counter;
			a += yincr[k];
			b += xincr[k];
			if(onBoard(a,b)) curr = (*board)[a][b];
		}
		// if we reach the end of the board or curr isn't open space at end of line, this is a closed row
		if(!onBoard(a,b) || curr != '+') open = false;
		// look at opposite side
		a = i-yincr[k];
		b = j-xincr[k];
		curr = '+';
		if(onBoard(a,b)) curr = (*board)[a][b];
		while(onBoard(a,b) && curr == match) {
			++counter;
			a -= yincr[k];
			b -= xincr[k];
			if(onBoard(a,b)) curr = (*board)[a][b];
		}
		if(!onBoard(a,b) || curr != '+') open = false;
		if(counter >= 5) {
			win = true;
			if (counter == 6) return pair<bool,bool>(false,false); 
		}
		if(open && counter == 3) ++openThree;
	}
	if(openThree == 2) return pair<bool,bool>(false,win); // assuming moves that create 3 or 4 open threes are allowed
	return pair<bool,bool>(true,win);
}
