#ifndef HIVE_HIVE_H
#define HIVE_HIVE_H

#include "HexGrid.h"
#include <vector>
#include <array>
#include <queue>

using std::array;
using std::vector;
using std::queue;

namespace Hive
{

	class Game 
	{
		public:
			Game(Piece player_first_piece);
			const HexGrid& get_grid() const { return grid; }
			vector<Hex> valid_moves(Hex p);
			vector<Hex> valid_spawns(Color color);
		private:
			bool is_locked(Hex p);
			bool is_outside(Hex p); 
			bool has_neighbour_with_color(Hex p, Color color);
			bool can_move(Color color);
			inline void spawn(int x, int y, Color color, Piece piece);
			vector<Hex> ant_valid_moves(Hex p);
			vector<Hex> bee_valid_moves(Hex p);
			vector<Hex> beetle_valid_moves(Hex p);
			vector<Hex> grasshopper_valid_moves(Hex p);
			vector<Hex> spider_valid_moves(Hex p);
			HexGrid grid;
			array<bool,2> queen_spawned;
			const array<array<Hex,6>,2> dirs = {{
				{{ // Even x:
					Hex(0, -1),
					Hex(+1, 0),
					Hex(+1, +1),
					Hex(0, +1),
					Hex(-1, +1),
					Hex(-1, 0)
				}},
				{{ // Odd x:
					Hex(0, -1),
					Hex(+1, -1),
					Hex(+1, 0),
					Hex(0, +1),
					Hex(-1, 0),
					Hex(-1, -1)
				}}
			}};

	};

	Game::Game(Piece player_first_piece)
	{
		assert(player_first_piece != Piece::NoPiece);
		queen_spawned[0] = queen_spawned[1] = false;
		spawn(GSIDE/2, GSIDE/2-1, Color::Black, Piece::Ant); // TODO: IA
		spawn(GSIDE/2, GSIDE/2, Color::White, player_first_piece);
	}

	bool Game::is_locked(Hex p)
	{
		return p.layer == 0 and grid[p.x][p.y][1].piece != Piece::NoPiece;
	}

	bool Game::is_outside(Hex p)
	{
		if (p.layer < 0 or p.layer > 1) return true;
		if (p.x < 0 or p.x >= GSIDE) return true;
		if (p.y < 0 or p.y >= GSIDE) return true;
		return false;
	}

	bool Game::has_neighbour_with_color(Hex p, Color color)
	{
		for (const Hex& dir : dirs[p.x%2]) {
			Hex h = p + dir;
			if (not is_outside(h)) {
				if (grid[h.x][h.y][0].color == color) return true;
				if (grid[h.x][h.y][1].color == color) return true;
			}
 		}
 		return false;
	}

	bool Game::can_move(Color color)
	{
		if (color == Color::NoColor) {
			return false;
		}
		return queen_spawned[(int)color];
	}

	inline void Game::spawn(int x, int y, Color color, Piece piece)
	{
		std::cout << "Used: " << x << ", " << y << std::endl; //
		if (piece == Piece::Bee) {
			queen_spawned[(int)color] = true;
		}
		grid[x][y][0] = Hex(0, color, x, y, piece);
	}

	vector<Hex> Game::valid_spawns(Color color)
	{
		assert(color == 0 or color == 1);

		vector<Hex> vs; // valid spawns
		vector<vector<bool>> visited(GSIDE, vector<bool>(GSIDE, false));
		queue<Hex> q;
		q.push(Hex(GSIDE/2, GSIDE/2));
		while (not q.empty()) {
			Hex h = q.front();
			q.pop();
			for (const Hex& dir : dirs[h.x%2]) {
				Hex h2 = h + dir;
				if (not is_outside(h2) and not visited[h2.x][h2.y]) {
					if (grid[h2.x][h2.y][0].piece == Piece::NoPiece
						and not has_neighbour_with_color(h2, (Color)!color)) 
					{
						vs.push_back(h2);
					}
					visited[h2.x][h2.y] = true;
					if (grid[h2.x][h2.y][0].piece != Piece::NoPiece)
					{
						q.push(h2);
					}
				}
			}
		}
		return vs;
	}

	vector<Hex> Game::ant_valid_moves(Hex p)
	{
		return vector<Hex>(); // TODO: implement
	}

	vector<Hex> Game::bee_valid_moves(Hex p)
	{
		return vector<Hex>(); // TODO: implement
	}
	
	vector<Hex> Game::beetle_valid_moves(Hex p)
	{
		return vector<Hex>(); // TODO: implement
	}
	
	vector<Hex> Game::grasshopper_valid_moves(Hex p)
	{
		return vector<Hex>(); // TODO: implement
	}
	
	vector<Hex> Game::spider_valid_moves(Hex p)
	{
		return vector<Hex>(); // TODO: implement
	}

	vector<Hex> Game::valid_moves(Hex p)
	{
		assert(p.piece != Piece::NoPiece);

		if (is_locked(p)) {
			return vector<Hex>();
		}
		
		switch(p.piece) {
			case Ant:
				return ant_valid_moves(p);
			case Bee:
				return bee_valid_moves(p);
			case Beetle:
				return beetle_valid_moves(p);
			case Grasshopper: 
				return grasshopper_valid_moves(p);
			case Spider:
				return spider_valid_moves(p);
		}

		assert(false);
		return vector<Hex>(); // Fatal error
	}

}

#endif