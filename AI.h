#ifndef HIVE_AI_H
#define HIVE_AI_H

#include "Hive.h"
#include <cmath>
#include <ctime>
#include <map>
#include <chrono>

namespace AI
{
	typedef long long ll; // easier to type
	typedef unsigned long long ull; // easier to type
	typedef long double ld; // easier to type
	template <typename T> using V = std::vector<T>; // easier to type
	using namespace std;
	using namespace Hive;

	enum PlayType { NoPlay, Put, Move };

	struct PlayInfo {
		PlayType type;
		ll score;
		Hex h;
		Piece piece; // PlayType == Put
		Hex h2; 	 // PlayType == Move
	};

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

	inline int delta_time(const clock_t& time0)
	{
		return (clock() - time0) * 1000 / CLOCKS_PER_SEC;
	}

	inline void reset_clock(clock_t& time0)
	{
		time0 = clock();
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

	PlayInfo play_info_move(ll score, Hex h, Hex h2, Piece piece)
	{
		PlayInfo p;
		p.type = PlayType::Move;
		p.score = score;
		p.h = h;
		p.h2 = h2;
		p.piece = piece;
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

	PlayInfo gen_random_play_put(Game& game, Color color)
	{
		V<Hex> vspawns = game.valid_spawns(color);
		random_shuffle(vspawns.begin(), vspawns.end());
		array<Piece,5> pieces = PIECES;
		random_shuffle(pieces.begin(), pieces.end());
		for (Piece piece : pieces) {
			if (game.pieces_left[color][piece] == 0) continue;
			for (Hex p : vspawns) {
				if (game.is_outside(Hex(0, p.x, p.y))) continue;
				if (piece != Piece::Bee && !game.bee_spawned[color]
					&& NPIECERPERPLAYER - game.total_pieces_left[color] >= 3) 
				{
					continue;
				}

				return play_info_put(0, p, piece);
			}
		}

		return play_info_null();
	}

	PlayInfo gen_random_play_move(Game& game, Color color)
	{
		array<Piece,5> pieces = PIECES;
		random_shuffle(pieces.begin(), pieces.end());
		for (Piece piece : pieces) {
			for (Hex h : game.positions[color][piece]) {
				if (game.is_locked(h)) continue;
				V<Hex> valid_moves = game.valid_moves(h);
				random_shuffle(valid_moves.begin(), valid_moves.end());
				for (Hex p : valid_moves) {
					if (game.is_outside(Hex(p.layer, p.x, p.y))) continue;
					if (game.grid[p.x][p.y][p.layer].piece != Piece::NoPiece) continue;

					return play_info_move(0, h, p, piece);
				}	
			}
		}

		return play_info_null();
	}

	PlayInfo gen_random_play(Game& game, Color color)
	{
		if (rand() % 2) {
			PlayInfo play = gen_random_play_put(game, color);
			if (play.type != PlayType::NoPlay) return play;
			return gen_random_play_move(game, color);
		}
		else {
			PlayInfo play = gen_random_play_move(game, color);
			if (play.type != PlayType::NoPlay) return play;
			return gen_random_play_put(game, color);
		}
	}

	V<PlayInfo> gen_plays(Game& game, Color color)
	{
		V<PlayInfo> plays;
		plays.reserve(128);

		V<Hex> vspawns = game.valid_spawns(color);
		for (Piece piece : PIECES) {
			if (game.pieces_left[color][piece] == 0) continue;
			for (Hex p : vspawns) {
				if (game.is_outside(Hex(0, p.x, p.y))) continue;
				if (piece != Piece::Bee && !game.bee_spawned[color]
					&& NPIECERPERPLAYER - game.total_pieces_left[color] >= 3) 
				{
					continue;
				}

				plays.push_back(play_info_put(0, p, piece));
			}
		}

		
		for (Piece piece : PIECES) {
			for (Hex h : game.positions[color][piece]) {
				if (game.is_locked(h)) continue;
				for (Hex p : game.valid_moves(h)) {
					if (game.is_outside(Hex(p.layer, p.x, p.y))) continue;
					if (game.grid[p.x][p.y][p.layer].piece != Piece::NoPiece) continue;

					plays.push_back(play_info_move(0, h, p, piece));
				}	
			}
		}

		random_shuffle(plays.begin(), plays.end());

		return plays;
	}

	bool do_play(Game& game, PlayInfo play, Color color)
	{
		if (play.type == PlayType::Put) {
			return game.put_piece(play.h.x, play.h.y, color, play.piece, true);
		}
		else if (play.type == PlayType::Move) {
			return game.move_piece(play.h2.x, play.h2.y, play.h, play.h2.layer, true);
		}
		if (DEBUG) cerr << "do_play() - ERROR: NO MOVE" << endl;
		return false;
	}

	bool undo_play(Game& game, PlayInfo play, Color color) 
	{
		if (play.type == PlayType::Put) { 
			game.destroy(Hex(0, play.h.x, play.h.y));
			return true;
		}
		else if (play.type == PlayType::Move) {
			game.destroy(play.h2); // always destroy first to avoid piece tracking errors
			game.spawn(play.h.x, play.h.y, color, play.piece, play.h.layer);
			return true;
		}
		if (DEBUG) cerr << "undo_play() - ERROR: NO MOVE" << endl;
		return false;
	}

}

#endif