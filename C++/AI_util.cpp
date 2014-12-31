#include "AI_util.h"

#include <cmath>
#include <algorithm>
#include <fstream>

Eint::Eint() : i(0), is_inf(false), is_neg_inf(false) { }

Eint::Eint(int init_i) : i(init_i), is_inf(false), is_neg_inf(false) { }

Eint::Eint(string s) : i(0) {
	if (s == "inf") {
		is_inf = true;
		is_neg_inf = false;
	} else if (s == "-inf") {
		is_inf = false;
		is_neg_inf = true;
	}
}

string Eint::str() const {
	if(is_inf) return "inf";
	if(is_neg_inf) return "-inf";
	else return to_string(i);
}

bool operator==(const Eint& left, const Eint& right) {
	return (left.is_inf == right.is_inf) && (left.is_neg_inf == right.is_neg_inf) && left.i == right.i;
}

bool operator!=(const Eint& left, const Eint& right) {
	return !(left == right);
}

bool operator>(const Eint& left, const Eint& right) {
	if(left.is_inf) {
		if(right.is_inf) return false;
		else return true;
	} else if(right.is_inf) {
		return false;
	} else if(right.is_neg_inf) {
		if(left.is_neg_inf) return false;
		else return true;
	} else if(left.is_neg_inf) {
		return false;
	} else return left.i > right.i;
}


bool operator<(const Eint& left, const Eint& right) {
	return right > left;
}

bool operator>=(const Eint& left, const Eint& right) {
	return !(left < right);
}

bool operator<=(const Eint& left, const Eint& right) {
	return !(left > right);
}

ostream& operator<<(ostream& out, const Eint& x) {
	return out << x.str();
}

State::State() {
	for(int i = 0; i < 15; ++i) {
		for(int j = 0; j < 15; ++j) {
			board[i][j] = '+';
		}
	}
}

State::State(char (*init_board)[15][15]) {
	set_board(init_board);
}

State::State(string filename) {
	char init_board[15][15];
	ifstream in;
	in.open(filename);
	int i = 0;
	int j = 0;
	while(in.good()) {
		char c = in.get();
		if(in.good()) {
			if (c == '+' || c == '@' || c == 'O') {
				init_board[i][j] = c;
				++j;
			}
			if (j == 15) {
				j = 0;
				++i;
			}
		}
	}
	cout << endl;
	set_board(&init_board);
}

bool State::on_board(int i, int j) const {
	return (i >= 0 && j >= 0 && i < 15 && j < 15);
}

void State::set_adj(int i, int j) {
	if(on_board(i,j) && board[i][j] == '+') {
		adj_moves.insert(space(i,j));
	}
}

void State::set_board(char (*init_board)[15][15]) {
	while(adj_moves.size() > 0) adj_moves.clear();
	// copy board
	for(int i = 0; i < 15; ++i) {
		for(int j = 0; j < 15; ++j) {
			board[i][j] = (*init_board)[i][j];
		}
	}
	// determine adjacent pieces
	for(int i = 0; i < 15; ++i) {
		for(int j = 0; j < 15; ++j) {
			if(board[i][j] != '+') {
				for(int a = i-1; a <= i+1; ++a)
					for(int b = j-1; b <= j+1; ++b)
						set_adj(a,b);
			}
		}
	}
}

void State::put(int i, int j, player p) {
	if (board[i][j] != '+') {
		cerr << "board full!" << i << ' ' << j << endl;
		return;
	}
	// remove this space from the adjacent moves
	adj_moves.erase(space(i,j));
	// add peice to board
	board[i][j] = (p == _AI) ? '@' : 'O';
	// add adjacent spaces to adjacent set
	for(int a = i-1; a <= i+1; ++a)
		for(int b = j-1; b <= j+1; ++b)
			set_adj(a,b);
}

void State::put(space s, player p) {
	put(s.first,s.second,p);
}

pair<bool,bool> State::legalwin(int i, int j, player p) const {
	// if space is occupied our outside board boundary return false
	if (!on_board(i,j) || board[i][j] != '+') return pair<bool,bool>(false,false);
	// check if the move violates three and three rule
	bool win = false;
	int open_three = 0; // open three rows
	char match = (p == _AI) ? '@' : 'O';
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
		if(on_board(a,b)) curr = board[a][b];
		while(on_board(a,b) && curr == match) {
			++counter;
			a += yincr[k];
			b += xincr[k];
			if(on_board(a,b)) curr = board[a][b];
		}
		// if we reach the end of the board or curr isn't open space at end of line, this is a closed row
		if(!on_board(a,b) || curr != '+') open = false;
		// look at opposite side
		a = i-yincr[k];
		b = j-xincr[k];
		curr = '+';
		if(on_board(a,b)) curr = board[a][b];
		while(on_board(a,b) && curr == match) {
			++counter;
			a -= yincr[k];
			b -= xincr[k];
			if(on_board(a,b)) curr = board[a][b];
		}
		if(!on_board(a,b) || curr != '+') open = false;
		if(counter >= 5) {
			if (counter == 6) return pair<bool,bool>(false,false); 
			else win = true;
		}
		if(open && counter == 3) ++open_three;
		//if(open_three == 2) return false; //uncomment if we don't allow 2 or more open rows
	}
	if(open_three == 2) return pair<bool,bool>(false,win); // assuming moves that create 3 or 4 open threes are allowed
	return pair<bool,bool>(true,win);
}

State State::next(int i, int j, player p) const {
	State result = *this;
	result.put(i,j,p);
	return result;
}

int State::i_row_value(bool last_p, int count, int num_closed) const {
	if(num_closed >= 2) return 0;
	if(last_p) {
		if (count == 4 && num_closed == 0) return 2048;
		else if (count == 4 && num_closed == 1) return 128;
		else if (count == 3 && num_closed == 0) return 64;
		else if (count == 3 && num_closed == 1) return 16;
		else if (count == 2 && num_closed == 0) return 8;
		else if (count == 2 && num_closed == 1) return 4;
	}
	else {
		if (count == 4 && num_closed == 0) return 4096;
		else if (count == 4 && num_closed == 1) return 4096;
		else if (count == 3 && num_closed == 0) return 1024;
		else if (count == 3 && num_closed == 1) return 64;
		else if (count == 2 && num_closed == 0) return 16;
		else if (count == 2 && num_closed == 1) return 8;
	}
	return 0;
}

Eint State::line_value(player last_p, int i, int j, int yincr, int xincr) const {
	int a = i, b = j;
	char last_p_ch = (last_p == _AI) ? '@' : 'O';
	char curr, prev = '+';
	bool closed_prev = true;
	int counter = 0;
	int val = 0;

	while(on_board(a,b)) {
		curr = board[a][b];
		if (curr != '+' && curr == prev) {
			counter++;
			// if there is a 5-row whoever has it has won, so returns an infinite value
			if (counter >= 5) {
				if (curr == '@')
					return Eint("inf");
				else
					return Eint("-inf");
			}
		} else if (prev != '+') {
			// number of closed ends
			int num_closed = (closed_prev) ? 1 : 0;
			if (curr != '+') {
				++num_closed;	
				// if this space is not open the next streak is closed on prev end
				closed_prev = true;
			} else closed_prev = false;
			int incr = i_row_value((prev == last_p_ch),counter,num_closed);
			if (prev == 'O') incr *= -1;
			val += incr;
			/*
			if (counter > 1) {
				if (num_closed < 2) {
					incr = (int)pow(16,counter);
					if(num_closed == 1) incr /= 2;
					if(prev == last_p_ch) incr /= 8;
					if(prev == 'O') incr *= -1;
				}
			}
			*/
			if (curr != '+') counter = 1;
			else counter = 0;
		} else if (curr != '+') counter = 1;
		if (curr == '+') closed_prev = false;
		prev = curr;
		a += yincr;
		b += xincr;
	} 
	if (curr != '+') {
		// number of closed ends
		int num_closed = (closed_prev) ? 1 : 0;
		++num_closed;	
		int incr = i_row_value((prev == last_p_ch),counter,num_closed);
		val += incr;
		if (prev == 'O') incr *= -1;
		/*
		if (counter > 1) {
			int incr = 0;
			if (num_closed < 2) {
				incr = (int)pow(16,counter);
				if(num_closed == 1) incr /= 2;
				if(prev == last_p_ch) incr /= 8;
				if(prev == 'O') incr *= -1;
			}
			val += incr;
		}
		*/
	}
	return Eint(val);
}

Eint State::value(player last_p) const {
	int AI_rows[8];
	int hum_rows[8];
	for (int i = 0; i < 8; ++i) {
		AI_rows[i] = 0;
		hum_rows[i] = 0;
	}
	// these arrays store the four line directions
	int yincr[5] = { 0, -1, 1, -1 };
	int xincr[5] = { 1, 1, 1, 0 };
	int i, j;
	int val = 0;
	// check rows/diagonals that intersect with left side of board
	j = 0;
	for(int i = 0; i < 15; ++i) {
		for(int k = 0; k < 3; ++k) {
			Eint e = line_value(last_p,i,j,yincr[k],xincr[k]);
			// if a player has won immediately return that infinite value
			if (e.is_ext()) return e;
			else val += e.val();
		}
	}
	// check columns/diagonals that intersect with bottom side of board
	i = 14;
	for(int j = 1; j < 15; ++j) {
		for(int k = 1; k < 4; ++k) {
			Eint e = line_value(last_p,i,j,yincr[k],xincr[k]);
			if (e.is_ext()) return e;
			else val += e.val();
		}
	}
	// check columns/diagonals that intersect with top side of board
	i = 0;
	for(int j = 1; j < 15; ++j) {
		for(int k = 2; k < 3; ++k) {
			Eint e = line_value(last_p,i,j,yincr[k],xincr[k]);
			if (e.is_ext()) return e;
			else val += e.val();
		}
	}
	return Eint(val);
}

ostream& operator<<(ostream& out, const State& S) {
	string top = "  1 2 3 4 5 6 7 8 9 a b c d e f ";
	string space = "    ";
	out << top << endl; // << space << top << endl;
	out << hex;
	for(int i = 0; i < 15; ++i) {
		out << (i+1) << ' ';
		for(int j = 0; j < 15; ++j)
			out << S.board[i][j] << ' ';
		out << endl;
	}
	out << dec;
	return out;
}

bool operator==(const State& left, const State& right) {
	return left.board_string() == right.board_string();
}

string State::board_string() const {
	string result;
	for(int i = 0; i < 15; ++i) {
		result += string(board[i]);
	}
	return result;
}