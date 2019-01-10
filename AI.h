#ifndef HIVE_AI_H
#define HIVE_AI_H

#include "Hive.h"
#include <cmath>
#include <ctime>

#define USE_HEURISTIC 1

namespace AI
{
	typedef long long ll; // easier to type
	using std::endl; // DEBUG
	using std::array;
	using std::max;
	using std::min;
	using std::abs;
	template <typename T> using V = std::vector<T>; // easier to type
	using namespace Hive;

	enum PlayType { NoPlay, Put, Move };

	struct PlayInfo {
		PlayType type;
		ll score;
		Hex h;
		Piece piece; // PlayType == Put
		Hex h2; 	 // PlayType == Move
	};

	clock_t start;

	std::ostream& operator<<(std::ostream& os, const PlayInfo& p)
    {  
        os << '{';
		if (p.type == PlayType::Put) {
			os << "Put, " << p.score << ", " << p.h << ", " << int(p.piece);
		}
		else if (p.type == PlayType::Move) {
			os << "Move, " << p.score << ", " << p.h << ", " << p.h2;
		}
		else {
			os << "NoPlay, " << p.score;
		}
		os << '}';
        return os;  
    }

	bool operator==(const PlayInfo& a, const PlayInfo& b)
	{
		if (a.score == b.score) return true;
		return false;
	}

	bool operator<(const PlayInfo& a, const PlayInfo& b)
	{
		if (a.score < b.score) return true;
		return false;
	}

	bool operator>(const PlayInfo& a, const PlayInfo& b)
	{
		if (a.score > b.score) return true;
		return false;
	}

	inline int delta_time()
	{
		return (clock() - start) * 1000 / CLOCKS_PER_SEC;
	}

	inline void reset_clock()
	{
		start = clock();
	}


	PlayInfo play_info_put(ll score, Hex h, Piece piece)
	{
		PlayInfo p;
		p.type = PlayType::Put;
		p.score = score;
		p.h = h;
		p.piece = piece;
		return p;
	}

	PlayInfo play_info_move(ll score, Hex h, Hex h2)
	{
		PlayInfo p;
		p.type = PlayType::Move;
		p.score = score;
		p.h = h;
		p.h2 = h2;
		return p;
	}

	inline int rand_int(int m, int M)
	{
		return m + rand() % (M - m + 1);
	}

	ll get_heuristic_score_for_color(Game& game, Color color)
	{
		ll score = 0;

		score += pow10[6] * game.bee_spawned[color];

		if (game.bee_spawned[color]) {
			int surrounding_cnt = game.surrounding_cnt(game.positions[color][Piece::Bee][0]);
			score -= pow10[3] * (surrounding_cnt - 1);
		}

		int pieces_val = 0;
		for (Piece piece : PIECES) {
			pieces_val += PIECEVAL[piece] * game.positions[color][piece].size();
		}
		score += pow10[0] * pieces_val;

		return score;
	}

	ll get_heuristic_score(Game& game)
	{
		ll score = get_heuristic_score_for_color(game, ia_color) - get_heuristic_score_for_color(game, player_color);
		// D(score) << endl;
		return score;
	}

	PlayInfo minimax(Game& game, Color color, int depth, ll alpha, ll beta)
	{
		assert(depth <= MAXDEPTH);

		PlayInfo best_play;
		best_play.type = PlayType::NoPlay;
		best_play.score = (color == ia_color ? -LINF : LINF);

		Color winner = game.winner();
		// D(winner) << endl;
		if (winner != Color::NoColor) {
			best_play.score = (color == ia_color ? LINF : -LINF);
			return best_play;
		}

		V<PlayInfo> plays;
		plays.reserve(256);

		V<Hex> vspawns = game.valid_spawns(color);
		for (Piece piece : PIECES) {
			for (Hex p : vspawns) {
				plays.push_back(play_info_put(0, p, piece));
			}
		}

		for (Piece piece : PIECES) {
			for (Hex h : game.positions[color][piece]) {
				for (Hex p : game.valid_moves(h)) {
					plays.push_back(play_info_move(0, h, p));
				}	
			}
		}
		// for (int layer = 0; layer < 2; ++layer) {
		// 	for (int y = 0; y < GSIDE; ++y) {
		// 		for (int x = 0; x < GSIDE; ++x) {
		// 			Hex h = game.grid[x][y][layer];
		// 			if (h.color == color) {
		// 				for (Hex p : game.valid_moves(h)) {
		// 					plays.push_back(play_info_move(0, h, p));
		// 				}
		// 			}
		// 		}
		// 	}
		// }

		random_shuffle(plays.begin(), plays.end());

		while (not plays.empty()) {
			PlayInfo play = plays.back();
			plays.pop_back();

			bool valid;
			if (play.type == PlayType::Put) {
				valid = game.put_piece(play.h.x, play.h.y, color, play.piece, true);
			}
			else {
				valid = game.move_piece(play.h2.x, play.h2.y, play.h, play.h2.layer, true);
			}

			if (valid) {
				if (depth == MAXDEPTH or (depth == MAXDEPTH-1 and delta_time() > 4000)) {
#if USE_HEURISTIC
					play.score = get_heuristic_score(game);
#else // use montecarlo:
					play.score = monte_carlo_score(game);
#endif
				}
				else {
					play.score = minimax(game, (Color)!color, depth+1, alpha, beta).score;
				}
				// D(play.score) << endl;

				if (color == ia_color) { // maximize
					if (play > best_play) best_play = play;
					alpha = max(alpha, best_play.score);
				}
				else { // minimize
					if (play < best_play) best_play = play;
					beta = min(beta, best_play.score);
				}
				
				// Restore:
				if (play.type == PlayType::Put) { 
					game.destroy(Hex(0, play.h.x, play.h.y));
				}
				else {
					game.destroy(play.h2); // always destroy first to avoid piece tracking errors
					game.spawn(play.h.x, play.h.y, play.h.color, play.h.piece, play.h.layer);
				}

				if (beta <= alpha) return best_play;
			}
		}

		return best_play;
	}

	void play(Game& game)
	{
		reset_clock();
		PlayInfo play = minimax(game, ia_color, 1, -LINF, LINF);
		D(play) << endl;
		D(delta_time()) << endl;
		if (play.type == PlayType::Put) {
			game.put_piece(play.h.x, play.h.y, ia_color, play.piece);
		}
		else if (play.type == PlayType::Move) {
			game.move_piece(play.h2.x, play.h2.y, play.h, play.h2.layer);
		}
		else {
			// unable to move. TODO: check this case
		}
	}

}

#endif