#ifndef HIVE_AI_H
#define HIVE_AI_H

#include "Hive.h"
#include <cmath>
#include <ctime>
#include <map>

namespace AI
{
	typedef long long ll; // easier to type
	typedef unsigned long long ull; // easier to type
	using std::endl; // DEBUG
	using std::array;
	using std::max;
	using std::min;
	using std::abs;
	using std::map;
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

	clock_t __time0__;

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
		return (clock() - __time0__) * 1000 / CLOCKS_PER_SEC;
	}

	inline void reset_clock()
	{
		__time0__ = clock();
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

	PlayInfo play_info_null()
	{
		PlayInfo p;
		p.type = PlayType::NoPlay;
		p.score = 0;
		return p;
	}

	inline int rand_int(int m, int M)
	{
		return m + rand() % (M - m + 1);
	}

	ll get_heuristic_score_for_color(Game& game, Color color)
	{
		ll score = 0;

		if (game.bee_spawned[color]) {
			int surrounding_cnt = game.surrounding_cnt(game.positions[color][Piece::Bee][0]);
			score -= pow10[2] * (surrounding_cnt - 1);
		}

		for (Piece piece : PIECES) {
			score += pow10[1] * PIECEVAL[piece] * game.positions[color][piece].size();
		}

		// for (Piece piece : PIECES) {
		// 	for (Hex h : game.positions[color][piece]) {
		// 		score += 
		// 	}
		// }

		return score;
	}

	ll get_heuristic_score(Game& game)
	{
		ll score = get_heuristic_score_for_color(game, ia_color) - get_heuristic_score_for_color(game, player_color);
		// if (DEBUG) D(score) << endl;
		return score;
	}

	V<PlayInfo> gen_plays(Game& game, Color color)
	{
		V<PlayInfo> plays;
		plays.reserve(128);

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

		random_shuffle(plays.begin(), plays.end());

		return plays;
	}

}

#endif