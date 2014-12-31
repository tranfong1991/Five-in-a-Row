#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <vector>

#include "AI_util.h"
#include "AI.h"

AI::AI() : ab_count(0), dif(_EASY) { }

AI::AI(AI_dif i_dif) : ab_count(0), dif(i_dif) { }

AI::AI(char (*board)[15][15], AI_dif i_dif) : ab_count(0), dif(i_dif) { 
	current_state.set_board(board);
}

AI::AI(string filename, AI_dif i_dif) : ab_count(0), dif(i_dif) { 
	current_state = State(filename);
}

void AI::set_diff(AI_dif i_dif) {
	dif = i_dif;
}

void AI::set_board(char (*board)[15][15]) {
	current_state.set_board(board);
}

void AI::put_move(int i, int j, player p) {
	current_state.put(i,j,p);
}

void AI::put_move(space s, player p) {
	current_state.put(s.first,s.second,p);
}

space AI::get_move() {
	if(dif == _EASY) return alpha_beta(1);
	else if(dif == _MEDIUM) return alpha_beta(2);
	else return alpha_beta(4);
}

space AI::rand_legal_adj() { //careful, this will generate an infinite loop if no legal moves are avaliable
	srand(time(NULL));
	int n = current_state.adj_size();
	// if no pieces on board, start at a random location
	if(n == 0) {
		return space(4+(rand()%7),4+(rand()%7)); 
	}
	int k = rand() % n;
	unordered_set<space>::const_iterator it = current_state.adj_cbegin();
	for(int i = 0; i < k; ++i) ++it;
	// get rand move, if it isn't legal get next move, continue
	while(true) {
		if(current_state.legalwin(it->first, it->second, _AI).first)
			return (*it);
		else {
			++it;
			if (it == current_state.adj_cend()) 
				it = current_state.adj_cbegin();
		}
	}
}

struct ab_helper {
	Eint val;
	State st;
	space mv;
	ab_helper() { }
	ab_helper(Eint i_val, State i_st, space i_mv) : val(i_val), st(i_st), mv(i_mv) { }
};

bool highlow(const ab_helper& left, const ab_helper& right) {
	return left.val > right.val;
}

bool lowhigh(const ab_helper& left, const ab_helper& right) {
	return left.val < right.val;
}

Eint maxval(vector<ab_helper> v) {
	Eint max = v[0].val;
	for(int i = 0; i < v.size(); ++i) 
		if (v[i].val > max) max = v[i].val;
	return max;
}

Eint minval(vector<ab_helper> v) {
	Eint min = v[0].val;
	for(int i = 0; i < v.size(); ++i) 
		if (v[i].val < min) min = v[i].val;
	return min;
}

space AI::alpha_beta(int h) {
	//ab_count=0;
	srand(time(NULL));
	Eint alpha("-inf"), beta("inf");
	space best_move(-1,-1); 
	// generate child nodes for all possible adjacent moves	
	if (current_state.adj_size() <= 8)
		return rand_legal_adj();
	vector<ab_helper> v;
	unordered_set<space>::const_iterator it = current_state.adj_cbegin();
	while(it != current_state.adj_cend()) {
		int i = it->first, j = it->second;
		pair<bool,bool> legalwin = current_state.legalwin(i,j,_AI);
		bool legal = legalwin.first, win = legalwin.second;
		if (legal) {
			if (win) return (*it);
			State next_state = current_state.next(i,j,_AI);
			v.push_back(ab_helper(next_state.value(_AI),next_state,(*it)));
			++ab_count;
		}
		++it;
	}
	sort(v.begin(),v.end(),highlow);
	for(int i = 0; i < v.size(); ++i) {
		if (alpha >= beta) break;
		Eint child = alpha_beta_eval(v[i].st,1,h,alpha,beta,false);
		// adjust alpha (which also is the best value at root node)
		//cout << i << ' ' << child << ' ' << v.size() << endl;
		if (child > alpha) {
			alpha = child;
			best_move = v[i].mv;
		} 
	}
	if (best_move == space(-1,-1)) 
		best_move = v[0].mv;
	return best_move;
}

Eint AI::alpha_beta_eval(const State& S, int d, int h, Eint p_alpha, Eint p_beta, bool print) {
	if (p_alpha >= p_beta) cout << "error" << endl;
	bool maxnode = (d % 2 == 0) ? true : false;
	if (d == h) return (maxnode) ? S.value(_HUMAN) : S.value(_AI);
	// else generate next generation of children
	Eint alpha = p_alpha, beta = p_beta;
	Eint best_val = (maxnode) ? Eint("-inf") : Eint("inf");
	// if max node player is AI, else human
	player p = (maxnode) ? _AI : _HUMAN;
	// generate child nodes for all possible adjacent moves
	unordered_set<space>::const_iterator it = S.adj_cbegin();
	// order roughly best moves to worst for first two levels of search
	space best_move(-1,-1); 
	if (d < 3) {
		vector<ab_helper> v;
		while(it != S.adj_cend()) {
			int i = it->first, j = it->second;
			pair<bool,bool> legalwin = S.legalwin(i,j,p);
			bool legal = legalwin.first, win = legalwin.second;
			if (legal) {
				if (win) {
					if(maxnode) return Eint("inf");
					else return Eint("-inf");
				}
				State next_state = S.next(i,j,p);
				v.push_back(ab_helper(next_state.value(p),next_state,(*it)));
				++ab_count;
			}
			++it;
		}
		if(d+1 == h) return (maxnode) ? maxval(v) : minval(v);
		if(maxnode) sort(v.begin(),v.end(),highlow);
		else 	    sort(v.begin(),v.end(),lowhigh);
		// now search the children nodes
		for(int i = 0; i < v.size(); ++i) {
			// if alpha > beta stop evaluating
			if (alpha >= beta) break;
			Eint child = alpha_beta_eval(v[i].st,d+1,h,alpha,beta,false);
			// adjust alpha (which also is the best value at root node)
			if (maxnode && child > best_val) {
				if (child.is_ext()) return child;
				best_val = child;
				best_move = v[i].mv;
				if(child > alpha) alpha = child;
			} else if (!maxnode && child < best_val)  {
				if (child.is_ext()) return child;
				best_val = child;
				best_move = v[i].mv;
				if(child < beta) beta = child;
			}
		}
	}
	else {
		while(it != S.adj_cend() && beta > alpha) {
			int i = it->first, j = it->second;
			pair<bool,bool> legalwin = S.legalwin(i,j,p);
			bool legal = legalwin.first, win = legalwin.second;
			if(legal) {
				if(win) {
					if(maxnode) {
						//cout << "AW";
						return Eint("inf");
					}
					else {
						//cout << "HW";
						return Eint("-inf");
					}
				}
				// generate call to child node for this move
				Eint child = alpha_beta_eval(S.next(i,j,p),d+1,h,alpha,beta,false);
				// adjust alpha, beta, and current value of this node
				if (maxnode && child > best_val) {
					if (child.is_ext()) return child;
					best_val = child;
					best_move = space(i,j);
					if(child > alpha) alpha = child;
				} else if (!maxnode && child < best_val)  {
					if (child.is_ext()) return child;
					best_val = child;
					best_move = space(i,j);
					if(child < beta) beta = child;
				}
			}
			++it;
		}
	}
	return best_val;
}