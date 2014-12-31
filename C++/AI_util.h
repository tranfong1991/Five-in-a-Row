#pragma once
#include <unordered_set>
#include <string>
#include <utility>
#include <iostream>

using namespace std;

typedef pair<int,int> space;

namespace std {
	template <> struct hash<space> {
		size_t operator()(const space & x) const
		{
			return hash<int>()(x.first*15+x.second);
		}
	};
}

enum player { _AI, _HUMAN };

// extended integers, the set Z U { infinity, -infinity }
class Eint {
private:
	int i;
	bool is_inf;
	bool is_neg_inf;
public:
	Eint();
	Eint(int i);
	Eint(string s);
	string str() const;
	int val() const { return i; }
	bool is_ext() { return is_inf || is_neg_inf; }

    friend bool operator==(const Eint& left, const Eint& right);
    friend bool operator!=(const Eint& left, const Eint& right);
    friend bool operator>(const Eint& left, const Eint& right);
    friend bool operator<(const Eint& left, const Eint& right);
    friend bool operator>=(const Eint& left, const Eint& right);
    friend bool operator<=(const Eint& left, const Eint& right);
	friend ostream& operator<<(ostream& out, const Eint& x);
};

bool operator==(const Eint& left, const Eint& right);
bool operator!=(const Eint& left, const Eint& right);
bool operator>(const Eint& left, const Eint& right);
bool operator<(const Eint& left, const Eint& right);
bool operator>=(const Eint& left, const Eint& right);
bool operator<=(const Eint& left, const Eint& right);
ostream& operator<<(ostream& out, const Eint& x);

// AI has a local game state which may return an extended integer value
class State {
private:
	char board[15][15];
	unordered_set<space> adj_moves;
	void set_adj(int i, int j);
	int i_row_value(bool last_p, int count, int num_closed) const;
	Eint line_value(player last_p, int i, int j, int xincr, int yincr) const; // value of all rows on a particular line (i,j) + k(xincr,yincr) on the board
public:
	State();
	State(string filename);
	State(char (*init_board)[15][15]);
	bool on_board(int i, int j) const;
	void set_board(char (*init_board)[15][15]);
	void put(int i, int j, player p); // place this piece on board
	void put(space s, player p); 
	pair<bool,bool> legalwin(int i, int j, player p) const; // is this move legal
	State next(int i, int j, player p) const; // next state with this move
	const unordered_set<space>& adj() const { return adj_moves; } // adjacent pieces (map for efficiency)
	unordered_set<space>::const_iterator adj_cbegin() const { return adj_moves.cbegin(); }
	unordered_set<space>::const_iterator adj_cend() const { return adj_moves.cend(); }
	int adj_size() const { return adj_moves.size(); }
	Eint value(player last_p) const; //value of the game state; higher means AI is winning, lower means human is winning

	friend ostream& operator<<(ostream& out, const State& S);
	friend bool operator==(const State& left, const State& right);
	string board_string() const;
};

ostream& operator<<(ostream& out, const State& S);
bool operator==(const State& left, const State& right);