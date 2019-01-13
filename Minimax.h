#ifndef HIVE_MINIMAX_H
#define HIVE_MINIMAX_H

#include "AI.h"

namespace Minimax
{
	using namespace AI;
	using namespace std;

	clock_t time0;
	map<ull,PlayInfo> TT[TT_size];

	PlayInfo minimax(Game& game, V<PlayInfo>& plays, Color color, int depth, int max_depth, ll alpha, ll beta)
	{
		assert(depth <= max_depth);

		if (delta_time(time0) >= TLE) return play_info_null();

		ull H = game.hash(depth); 
		int TT_idx = H % TT_size; // transposition table
		auto TTtree = TT[TT_idx];
		auto TT_it = TTtree.find(H);
		if (TT_it != TTtree.end()) {
			return TT_it->second;
		}

		PlayInfo best_play;
		best_play.type = PlayType::NoPlay;
		best_play.score = (color == ia_color ? -LINF : LINF);

		Color winner = game.winner();
		// if (DEBUG) D(winner) << endl;
		if (winner != Color::NoColor) {
			best_play.score = (winner == ia_color ? LINF : -LINF);
			return best_play;
		}

		for (PlayInfo& play : plays) {
			do_play(game, play, color);

			if (depth == max_depth) {
				play.score = get_heuristic_score(game);
			}
			else {
				V<PlayInfo> next_plays = gen_plays(game, (Color)!color);
				play.score = minimax(game, next_plays, (Color)!color, depth+1, max_depth, alpha, beta).score;
			}
			// if (DEBUG) D(play.score) << endl;

			if (color == ia_color) { // maximize
				if (play > best_play) best_play = play;
				alpha = max(alpha, best_play.score);
			}
			else { // minimize
				if (play < best_play) best_play = play;
				beta = min(beta, best_play.score);
			}
			
			undo_play(game, play, color);

			if (beta <= alpha) return best_play;
		}

		if (best_play.type == PlayType::NoPlay) {
			best_play.score = (color == ia_color ? -LINF : LINF);
		}
		TTtree[H] = best_play; // memoize
		return best_play;
	}

	void play_hive(Game& game)
	{
		reset_clock(time0);
		V<PlayInfo> plays = gen_plays(game, ia_color);
		PlayInfo best_play = play_info_null();
		best_play.score = -LINF;
		int max_depth = 1;
		for (max_depth = 1; delta_time(time0) < TLE; ++max_depth) { // iterative deepening
			for (int i = 0; i < TT_size; ++i) {
				TT[i].clear();
			}
			PlayInfo play = minimax(game, plays, ia_color, 0, max_depth, -LINF, LINF);
			if (play > best_play) best_play = play;
			sort(plays.begin(), plays.end(), [](const PlayInfo& a, const PlayInfo& b) {
				return a > b;
			});
		}
		if (DEBUG) D(delta_time(time0)), D(best_play), D(max_depth) << endl;
		if (best_play.type == PlayType::Put) {
			game.put_piece(best_play.h.x, best_play.h.y, ia_color, best_play.piece, true);
		}
		else if (best_play.type == PlayType::Move) {
			game.move_piece(best_play.h2.x, best_play.h2.y, best_play.h, best_play.h2.layer, true);
		}
		else {
			// unable to move. TODO: check this case
		}
	}
};

#endif