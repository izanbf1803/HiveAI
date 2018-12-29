#ifndef HIVE_HIVE_H
#define HIVE_HIVE_H

#include "HexGrid.h"
#include "DisjointSet.h"
#include <vector>
#include <array>
#include <queue>

namespace Hive
{
	using std::array;
	using std::vector;
	using std::queue;
	using DisjointSet::DisjointSet;

	class Game 
	{
		public:
			Game(Piece player_first_piece);
			vector<Hex> valid_moves(Hex p);
			vector<Hex> valid_spawns(Color color);
			bool is_locked(Hex p);
			bool is_outside(Hex p); 
			bool is_accessible(Hex p, Hex p2);
			bool has_neighbour_with_color(Hex p, Color color);
			bool can_move(Color color);
			bool put_piece(int x, int y, Color color, Piece piece);
			bool move_piece(int x, int y, Hex h);
			vector<Hex> get_hexs_with_color(Color color);
			vector<Hex> get_neighbours(Hex h);
			HexGrid grid;
		private:
			void spawn(int x, int y, Color color, Piece piece);
			vector<Hex> ant_valid_moves(Hex h);
			vector<Hex> bee_valid_moves(Hex h);
			vector<Hex> beetle_valid_moves(Hex h);
			vector<Hex> grasshopper_valid_moves(Hex h);
			vector<Hex> spider_valid_moves(Hex h);
			int count_components();
			array<bool,2> queen_spawned;
			const array<Hex,2> initial_pos = {{
				Hex(0, GSIDE/2, GSIDE/2-1), // Black
				Hex(0, GSIDE/2, GSIDE/2), // White
			}};
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
		spawn(initial_pos[ia_color].x, initial_pos[ia_color].y, ia_color, Piece::Ant); // TODO: IA
		spawn(initial_pos[player_color].x, initial_pos[player_color].y, player_color, player_first_piece);
	}

	bool Game::is_locked(Hex p)
	{
		if (bool locked = p.layer == 0 and grid[p.x][p.y][1].piece != Piece::NoPiece) return true;
		// TODO: lock articulation vertices
		return false;
	}

	bool Game::is_outside(Hex p)
	{
		if (p.layer < 0 or p.layer > 1) return true;
		if (p.x < 0 or p.x >= GSIDE) return true;
		if (p.y < 0 or p.y >= GSIDE) return true;
		return false;
	}

	bool Game::is_accessible(Hex p, Hex p2)
	{
		int dx = p2.x - p.x;
		int dy = p2.y - p.y;
		int index = -1;
		for (const Hex& dir : dirs[p.x%2]) {
			++index;
			if (dir.x == dx and dir.y == dy) break;
		}
		Hex p2_l = p + dirs[p.x%2][(index-1+6)%6];
		Hex p2_r = p + dirs[p.x%2][(index+1)%6];
		if (not is_outside(p2_l) and grid[p2_l].color == Color::NoColor) return true;
		if (not is_outside(p2_r) and grid[p2_r].color == Color::NoColor) return true;
		return false;
	}

	bool Game::has_neighbour_with_color(Hex p, Color color)
	{
		for (Hex h : get_neighbours(p)) {
			if (grid[h.x][h.y][1].color == color) {
				return true;
			}
			if (grid[h.x][h.y][1].color == Color::NoColor and grid[h.x][h.y][0].color == color) {
				return true;
			}
 		}
 		return false;
	}

	bool Game::can_move(Color color)
	{
		assert(color != Color::NoColor);
		return queen_spawned[int(color)];
	}

	bool Game::put_piece(int x, int y, Color color, Piece piece)
	{
		if (x < 0 or y < 0 or x >= GSIDE or y >= GSIDE) return false;
		
		Hex h = Hex(0, color, x, y, piece);

		bool is_valid = false;
		for (Hex pos : valid_spawns(color)) {
			if (pos.x == h.x and pos.y == h.y) {
				is_valid = true;
				break;
			}
		}

		if (not is_valid) return false;

		spawn(x, y, color, piece);
		return true;
	}

	bool Game::move_piece(int x, int y, Hex _h)
	{
		if (x < 0 or y < 0 or x >= GSIDE or y >= GSIDE) return false;

		if (is_locked(_h)) return false;

		if (grid[x][y][0].color != Color::NoColor) return false;

		vector<Hex> vm = valid_moves(_h);
		D(vm.size()) << std::endl;
		
		Hex h = Hex(0, _h.color, x, y, _h.piece);

		bool is_valid = false;
		for (Hex pos : vm) {
			if (pos.x == h.x and pos.y == h.y) {
				is_valid = true;
				break;
			}
		}

		if (not is_valid) return false;

		grid[x][y][0] = h;
		grid[_h.x][_h.y][0] = Hex(0, _h.x, _h.y);
		return true;
	}

	void Game::spawn(int x, int y, Color color, Piece piece)
	{
		std::cout << "Trying to spawn at: ", D(x), D(y) << std::endl; //
		if (piece == Piece::Bee) {
			queen_spawned[int(color)] = true;
		}
		grid[x][y][0] = Hex(0, color, x, y, piece);
	}

	vector<Hex> Game::valid_spawns(Color color)
	{
		assert(color == 0 or color == 1);

		vector<Hex> vs; // valid spawns
		vector<vector<bool>> visited(GSIDE, vector<bool>(GSIDE, false));
		queue<Hex> q;
		q.push(get_hexs_with_color(color)[0]);
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

	vector<Hex> Game::ant_valid_moves(Hex h0)
	{
		grid[h0] = Hex(h0.layer, h0.x, h0.y); // Temporally delete piece

		vector<Hex> v; // rechable hexs
		D(count_components()) << std::endl;
		if (count_components() == 1) {
			vector<vector<bool>> visited(GSIDE, vector<bool>(GSIDE, false));
			queue<Hex> q;
			for (Hex h_ : get_neighbours(h0)) { // Find BFS origin
				if (h_.piece != Piece::NoPiece) { 
					q.push(h_);
					break;
				}
			}
			while (not q.empty()) {
				Hex h = q.front();
				q.pop();
				for (Hex p : get_neighbours(h)) {
					if (not is_outside(p) and grid[p].color == Color::NoColor and not visited[p.x][p.y]) 
					{
						if (is_accessible(h, p)) {
							if (grid[p].piece == Piece::NoPiece
								and (has_neighbour_with_color(p, Color::White) or has_neighbour_with_color(p, Color::Black)))
							{
								visited[p.x][p.y] = true;
								v.push_back(p);
								q.push(p);
							}
						}
					}
				}
			}
		}

		grid[h0] = h0; // Restore piece
		return v;
	}

	vector<Hex> Game::bee_valid_moves(Hex h)
	{
		return vector<Hex>(); // TODO: implement
	}
	
	vector<Hex> Game::beetle_valid_moves(Hex h)
	{
		return vector<Hex>(); // TODO: implement
	}
	
	vector<Hex> Game::grasshopper_valid_moves(Hex h)
	{
		return vector<Hex>(); // TODO: implement
	}
	
	vector<Hex> Game::spider_valid_moves(Hex h)
	{
		return vector<Hex>(); // TODO: implement
	}

	vector<Hex> Game::valid_moves(Hex p)
	{
		assert(p.piece != Piece::NoPiece);
		assert(p.color != Color::NoColor);

		if (is_locked(p) or not can_move(p.color)) {
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

	vector<Hex> Game::get_hexs_with_color(Color color)
	{
		vector<Hex> v;
		for (int y = 0; y < GSIDE; ++y) {
			for (int x = 0; x < GSIDE; ++x) {
				if (grid[x][y][0].color == color) {
					v.push_back(grid[x][y][0]);
				}
			}
		}
		return v;
	}

	vector<Hex> Game::get_neighbours(Hex p)
	{
		// TODO: implement multiple layers
		vector<Hex> v;
		for (const Hex& dir : dirs[p.x%2]) {
			Hex h = p + dir;
			if (not is_outside(h)) {
				v.push_back(h);
			}
		}
		return v;
	}

	int Game::count_components()
	{
		// TODO: implement multiple layers
		DisjointSet ds;
		for (int y = 0; y < GSIDE; ++y) {
    		for (int x = 0; x < GSIDE; ++x) {
				Hex h = grid[x][y][0];
				if (h.piece != Piece::NoPiece) ds.add(h.id());
			}
		}
		for (int y = 0; y < GSIDE; ++y) {
    		for (int x = 0; x < GSIDE; ++x) {
				Hex h = grid[x][y][0];
				if (h.piece != Piece::NoPiece) {
					for (Hex p : get_neighbours(h)) {
						p = grid[p];
						if (p.piece != Piece::NoPiece) ds.merge(h.id(), p.id());
					}
				}
			}
		}
		return ds.count();
	}

}

#endif