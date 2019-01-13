#ifndef HIVE_MCTS_H
#define HIVE_MCTS_H

#include "AI.h"
#include <chrono>

namespace MCTS
{
	using namespace AI;
	using namespace std;

	clock_t time_;

	class Node {
		public:
			Node();
			void expand(Game& game);
			Node* select();
			ll simulate(Game& game, clock_t time0, Color color);
			void backpropagation(ll win);
			Node* play_hive(Game& game);
			void destroy();
			int visits;
			ll wins;
			bool expanded;
			Color color;
			PlayInfo play;
			Node* parent;
			V<Node*> childs;
		private:
			inline void set_parent(Node* _parent) { parent = _parent; };
			inline void set_play(PlayInfo _play) { play = _play; };
			inline void set_color(Color _color) { color = _color; };
			inline ld uct() const;
			Node* random_child();
	};

	Node::Node()
	{
		visits = 0;
		wins = 0;
		expanded = false;
		parent = NULL;
		childs = V<Node*>();
		color = Color::NoColor;
		play = play_info_null();
	}

	inline ld Node::uct() const
	{
		if (visits == 0) return INF;
		return (ld)wins / visits + C * sqrt(log(parent->visits) / visits);
	}

	Node* Node::select()
	{
		Node* best_node = NULL;
		ld best_uct = -INF;
		random_shuffle(childs.begin(), childs.end());
		for (Node* node : childs) {
			ld node_uct = node->uct();
			if (node_uct - best_uct > -EPS) { // update if greatest or equal score
				best_uct = node_uct;
				best_node = node;
			}
		}
		return best_node;
	}

	void Node::expand(Game& game)
	{
		if (expanded) return;
		// cout << "expand() - "; D(this), D(color), D(parent) << endl;
		V<PlayInfo> plays = gen_plays(game, color);
		for (PlayInfo p : plays) {
			// D(p) << endl;
			Node* child = new Node();
			child->set_parent(this);
			// D(this), D(child) << endl;
			child->set_play(p);
			child->set_color((Color)!color);
			childs.push_back(child);
		}
		expanded = true;
	}

	// ll Node::simulate(Game& game, clock_t time0, Color _color) // heuristic
	// {
	// 	const int depth = 32;
	// 	assert(depth%2==0); // to ensure _color is the same after iterations

	// 	V<PlayInfo> stack1;
	// 	V<Color> stack2;
	// 	stack1.reserve(depth);
	// 	stack2.reserve(depth);
	// 	bool end = false;
	// 	ll win = 0;

	// 	for (int i = 0; !end && i < depth; ++i) {
	// 		Color winner = game.winner();
	// 		if (winner != Color::NoColor) {
	// 			win = (winner == color ? pow10[5] : -pow10[5]);
	// 			end = true;
	// 		}

	// 		PlayInfo p = gen_random_play(game, _color);

	// 		if (p.type != PlayType::NoPlay) {
	// 			do_play(game, p, _color);
	// 			_color = (Color)!color;

	// 			stack1.push_back(p);
	// 			stack2.push_back(_color);
	// 		}
	// 		else {
	// 			end = true;
	// 		}
	// 	}

	// 	win = get_heuristic_score_for_color(game, _color);

	// 	while (!stack1.empty()) {
	// 		undo_play(game, stack1.back(), stack2.back());
	// 		stack1.pop_back();
	// 		stack2.pop_back();
	// 	}

	// 	return win;
	// }

	int Node::simulate(Game& game, clock_t time0, Color _color) binary -> win / lose
	{
		const int depth = 24;

		V<PlayInfo> stack1;
		V<Color> stack2;
		stack1.reserve(depth);
		stack2.reserve(depth);
		bool end = false;
		bool win = false;

		for (int i = 0; !end && i < depth; ++i) {
			Color winner = game.winner();
			if (winner != Color::NoColor) {
				win = winner == color;
				end = true;
			}

			PlayInfo p = gen_random_play(game, _color);

			if (p.type != PlayType::NoPlay) {
				do_play(game, p, _color);
				_color = (Color)!color;

				stack1.push_back(p);
				stack2.push_back(_color);
			}
			else {
				end = true;
			}
		}

		while (!stack1.empty()) {
			undo_play(game, stack1.back(), stack2.back());
			stack1.pop_back();
			stack2.pop_back();
		}
	}

	// bool Node::simulate(Game& game, clock_t time0, Color _color) // optimized for future search
	// {
	// 	V<PlayInfo> stack1;
	// 	V<Color> stack2;
	// 	stack1.reserve(64);
	// 	stack2.reserve(64);
	// 	bool end = false;
	// 	bool win = false;
	// 	Node* node = this;

	// 	for (int i = 0; !end && i < 16; ++i) {
	// 		node->expand(game);

	// 		Color winner = game.winner();
	// 		if (winner != Color::NoColor) {
	// 			win = winner == color;
	// 			end = true;
	// 		}

	// 		if (!node->childs.empty()) {
	// 			int idx = rand_int(0, node->childs.size()-1);
	// 			Node* child = node->childs[idx];
	// 			do_play(game, child->play, node->color);

	// 			stack1.push_back(node->play);
	// 			stack2.push_back(node->color);

	// 			node = child;
	// 		}
	// 		else {
	// 			end = true;
	// 		}
	// 	}

	// 	while (!stack1.empty()) {
	// 		undo_play(game, stack1.back(), stack2.back());
	// 		stack1.pop_back();
	// 		stack2.pop_back();
	// 	}

	// 	return win;
	// }

	void Node::backpropagation(ll win)
	{
		Node* node = this;
		while (node != NULL) {
			++node->visits;
			node->wins += win;
			node = node->parent;
		}
	}

	Node* Node::random_child()
	{
		if (childs.empty()) return NULL;
		return childs[rand_int(0, childs.size()-1)];
	}

	void Node::destroy()
	{
		for (Node* child : childs) {
			child->destroy();
		}
		delete this;
	}

	Node* Node::play_hive(Game& game)
	{
		// cout << "play_hive() - "; D(this) << endl;

		clock_t time0;
		clock_t simulation_time0;

		reset_clock(time0);

		expand(game);

		reset_clock(simulation_time0);

		// auto start = chrono::high_resolution_clock::now(); ///////////////////////////////////////
		// ...
		// auto elapsed = chrono::high_resolution_clock::now() - start; ////////////////////////////
		// long long microseconds = chrono::duration_cast<chrono::microseconds>(elapsed).count(); //
		// D(microseconds) << endl; ////////////////////////////////////////////////////////////////
		
		while (delta_time(time0) < TLE) {
			for (int i = 0; i < 16; ++i) {
				Node* promising = select();
				do_play(game, promising->play, (Color)!promising->color);
				promising->expand(game);
				Node* to_explore = promising;
				if (!promising->childs.empty()) {
					to_explore = promising->random_child();
				}

				if (to_explore != promising) do_play(game, to_explore->play, (Color)!to_explore->color);

				reset_clock(simulation_time0);
				ll win = to_explore->simulate(game, simulation_time0, (Color)!to_explore->color); 
				
				to_explore->backpropagation(win);

				// Restore:
				if (to_explore != promising) undo_play(game, to_explore->play, (Color)!to_explore->color);
				undo_play(game, promising->play, (Color)!promising->color);
			}
		}


		Node* best_node = NULL;
		ld best_score = -INF;

		int visits_sum = 0;

		for (Node* child : childs) {
			visits_sum += child->visits; /////////////////////
			ld score = child->uct();
			D(child->visits), D(child->play), D(score), D(child->wins) << endl;
			if (score - best_score > -EPS) {  // update if greatest or equal score
				best_score = score;
				best_node = child;
			}
		}

		D((ld)visits_sum/childs.size()) << endl;

		D(best_node) << endl;
		if (best_node != NULL) {
			D(best_node->play) << endl;
			do_play(game, best_node->play, ia_color);
		}

		for (Node* child : childs) {
			if (child != best_node) child->destroy();
		}

		return best_node;

		// if (DEBUG) D(delta_time()), D(max_depth) << endl;
	}

};

#endif