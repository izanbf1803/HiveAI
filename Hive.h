#ifndef HIVE_HIVE_H
#define HIVE_HIVE_H

#include "HexGrid.h"
#include <vector>
#include <array>

using std::array;
using std::vector;

namespace Hive 
{

	class Game 
	{
		public:
			Game(Piece player_first_piece);
			const HexGrid& get_grid() const { return grid; }
			vector<Hex> valid_moves(Hex p);
		private:
			bool is_locked(Hex p);
			vector<Hex> ant_valid_moves(Hex p);
			vector<Hex> bee_valid_moves(Hex p);
			vector<Hex> beetle_valid_moves(Hex p);
			vector<Hex> grasshopper_valid_moves(Hex p);
			vector<Hex> spider_valid_moves(Hex p);
			HexGrid grid;
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
		assert(player_first_piece != Piece::None);

		grid[11][10][0].piece = Piece::Ant; // TODO: IA
		grid[11][11][0].piece = player_first_piece;
	}

	bool Game::is_locked(Hex p)
	{
		return p.layer == 0 and grid[p.x][p.y][1].piece != Piece::None;
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
		assert(p.piece != Piece::None);

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

		return vector<Hex>(); // Fatal error
	}

}

#endif