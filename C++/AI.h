#pragma once
#include "AI_util.h"

enum AI_dif {
	_EASY, _MEDIUM, _HARD
};

class AI {
private:
	AI_dif dif;
	State current_state;
	space rand_legal_adj();
	space alpha_beta(int h);
	Eint alpha_beta_eval(const State& S, int d, int h, Eint alpha, Eint beta, bool presort);
public:
	int ab_count;
	AI();
	AI(AI_dif i_dif);
	AI(char (*board)[15][15], AI_dif i_dif);
	AI(string filename, AI_dif i_dif);
	void set_diff(AI_dif i_dif);
	void set_board(char (*board)[15][15]);
	void put_move(int i, int j, player p);
	void put_move(space s, player p);
	void print_board() { cout << current_state << endl; }
	space get_move();
};