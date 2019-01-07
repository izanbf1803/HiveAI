#ifndef HIVE_AI_H
#define HIVE_AI_H

#include "Hive.h"
#include <cmath>

namespace AI
{
	typedef long double ld; // easier to type
	using std::endl; // DEBUG
	using std::array;
	template <typename T> using V = std::vector<T>; // easier to type
	using namespace Hive;

	enum PlayType { NoPlay, Put, Move };

	struct PlayInfo {
		PlayType type;
		ld score;
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

	bool operator<(const PlayInfo& a, const PlayInfo& b)
	{
		if (a.score - b.score > EPS) return true;
		return false;
	}

	PlayInfo play_info_put(ld score, Hex h, Piece piece)
	{
		PlayInfo p;
		p.type = PlayType::Put;
		p.score = score;
		p.h = h;
		p.piece = piece;
		return p;
	}

	PlayInfo play_info_move(ld score, Hex h, Hex h2)
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

	ld get_heuristic_score(Game& game, int depth, Color color)
	{
		int cnt[2] = {0, 0};
		for (Color color : {Color::Black, Color::White}) {
			if (not game.positions[color][Piece::Bee].empty()) {
				cnt[color] = game.surrounding_cnt(game.positions[color][Piece::Bee][0]);
			}
		}
		int pieces_val = 0;
		for (Piece piece : PIECES) {
			pieces_val += PIECEVAL[piece] * game.positions[color][piece].size();
		}

		return (cnt[!int(color)] - cnt[color]) * pow(10.0L, 5) + pieces_val;
	}

	PlayInfo find_best_play(Game& game, Color color, int depth)
	{
		PlayInfo best;
		Color winner = game.winner();
		D(winner) << endl;
		if (winner == Color::NoColor) {
			V<PlayInfo> plays;
			V<Hex> vspawns = game.valid_spawns(color);
			for (Piece piece : PIECES) {
				for (Hex p : vspawns) {
					if (game.put_piece(p.x, p.y, color, piece, true)) {
						ld score = get_heuristic_score(game, depth, color);
						plays.push_back(play_info_put(score, p, piece));
						// Restore:
						game.destroy(Hex(0, p.x, p.y));
					}
				}
			}
			for (int layer = 0; layer < 2; ++layer) {
				for (int y = 0; y < GSIDE; ++y) {
					for (int x = 0; x < GSIDE; ++x) {
						Hex h = game.grid[x][y][layer];
						if (h.color == color) {
							V<Hex> vmoves = game.valid_moves(h);
							// D(h), D(h.piece), D(vmoves.size()) << endl;
							for (Hex p : vmoves) {
								if (game.move_piece(p.x, p.y, h, p.layer, true)) {
									ld score = get_heuristic_score(game, depth, color);
									plays.push_back(play_info_move(score, h, p));
									// Restore:
									game.destroy(p); // always destroy first to avoid piece tracking errors
									game.spawn(h.x, h.y, h.color, h.piece, h.layer);
								}
								// else {
								// 	D(h), D(p) << endl;
								// }
							}
						}
					}
				}
			}

			if (plays.empty()) {
				best.type = PlayType::NoPlay;
				best.score = 0;
			}
			else {
				best = plays[0];
				for (int i = 1; i < plays.size(); ++i) {
					if (plays[i] < best) best = plays[i];
				}
			}
		}
		else {
			best.type = PlayType::NoPlay;
			best.score = INF * (winner == color ? 1 : -1);
		}
		// for (PlayInfo play : plays) D(play) << endl;
		// D(best) << endl;
		return best;
	}

	void play(Game& game)
	{
		PlayInfo play = find_best_play(game, ia_color, 0);
		D(play) << endl;
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