#ifndef HIVE_HIVE_H
#define HIVE_HIVE_H

#include "HexGrid.h"
#include <queue>
#include <algorithm>
#include <cstring>

namespace Hive
{
	using std::array;
	using std::vector;
	using std::queue;
	using std::find;
	using std::max;
	using std::min;

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
			bool has_neighbour(Hex p);
			bool put_piece(int x, int y, Color color, Piece piece, bool validated = false);
			bool move_piece(int x, int y, Hex h, int layer = 0, bool validated = false);
			void spawn(int x, int y, Color color, Piece piece, int layer = 0);
			void destroy(Hex h);
			Color winner();
			int surrounding_cnt(Hex h);
			Hex get_hex_with_color(Color color);
			Hex get_hex_with_any_piece();
			unsigned long long hash(long long depth);
			vector<Hex> get_neighbours(Hex h, bool all_layers = false);
			array<array<vector<Hex>,NPIECETYPES>,2> positions; // color, position, index
			array<array<int,NPIECETYPES>,2> pieces_left;
			array<int,2> total_pieces_left;
			array<bool,2> bee_spawned;
			HexGrid grid;
		private:
			vector<Hex> ant_valid_moves(Hex h);
			vector<Hex> bee_valid_moves(Hex h);
			vector<Hex> beetle_valid_moves(Hex h);
			vector<Hex> grasshopper_valid_moves(Hex h);
			vector<Hex> spider_valid_moves(Hex h);
			int count_components();
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
		for (Color color : {Color::White, Color::Black}) {
			bee_spawned[color] = false;
			pieces_left[color][Piece::Ant] = 3;
			pieces_left[color][Piece::Bee] = 1;
			pieces_left[color][Piece::Beetle] = 2;
			pieces_left[color][Piece::Grasshopper] = 3;
			pieces_left[color][Piece::Spider] = 2;
			total_pieces_left[color] = NPIECERPERPLAYER;
		}
		spawn(initial_pos[ia_color].x, initial_pos[ia_color].y, ia_color, Piece::Spider); // TODO: IA
		spawn(initial_pos[player_color].x, initial_pos[player_color].y, player_color, player_first_piece);
	}

	bool Game::is_locked(Hex p)
	{
		if (p.layer == 0 and grid[p.x][p.y][1].piece != Piece::NoPiece) return true;
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
		p2_l.layer = p2_r.layer = p2.layer;
		if (not is_outside(p2_l) and grid[p2_l].piece == Piece::NoPiece) return true;
		if (not is_outside(p2_r) and grid[p2_r].piece == Piece::NoPiece) return true;
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

	bool Game::has_neighbour(Hex p)
	{
		for (Hex h : get_neighbours(p)) {
			if (grid[h].piece != Piece::NoPiece) return true;
		}
		return false;
	}

	bool Game::put_piece(int x, int y, Color color, Piece piece, bool validated)
	{
		if (pieces_left[color][piece] == 0) return false;
		if (is_outside(Hex(0, x, y))) return false;
		if (piece != Piece::Bee and not bee_spawned[color]
			and NPIECERPERPLAYER - total_pieces_left[color] >= 3) 
		{
			return false;
		}
		
		Hex h = Hex(0, color, x, y, piece);

		if (not validated) {
			bool is_valid = false;
			for (Hex pos : valid_spawns(color)) {
				if (pos.x == h.x and pos.y == h.y) {
					is_valid = true;
					break;
				}
			}

			if (not is_valid) return false;
		}

		spawn(x, y, color, piece);
		return true;
	}

	bool Game::move_piece(int x, int y, Hex _h, int layer, bool validated)
	{
		if (is_outside(Hex(layer, x, y))) return false;
		if (is_locked(_h)) return false;
		if (grid[x][y][layer].piece != Piece::NoPiece) return false;

		Hex h = Hex(layer, _h.color, x, y, _h.piece);

		if (not validated) {
			vector<Hex> vm = valid_moves(_h);

			bool is_valid = false;
			for (Hex pos : vm) {
				if (pos.x == h.x and pos.y == h.y) {
					is_valid = true;
					break;
				}
			}

			if (not is_valid) return false;
		}

		destroy(_h); // always destroy first to avoid piece tracking errors
		spawn(x, y, h.color, h.piece, h.layer);
		return true;
	}

	void Game::spawn(int x, int y, Color color, Piece piece, int layer)
	{
		assert(not is_outside(Hex(layer,x,y)) and piece != Piece::NoPiece and color != Color::NoColor);
		grid[x][y][layer] = Hex(layer, color, x, y, piece);
		if (piece == Piece::Bee) bee_spawned[color] = true;
		assert(pieces_left[color][piece] >= 0);
		--pieces_left[color][piece];
		--total_pieces_left[color];
		positions[color][piece].push_back(grid[x][y][layer]);
	}

	void Game::destroy(Hex h)
	{
		assert(not is_outside(h));
		h = grid[h];
		assert(h.piece != -1);
		grid[h] = Hex(h.layer, h.x, h.y);
		if (h.piece == Piece::Bee) bee_spawned[h.color] = false;
		++pieces_left[h.color][h.piece];
		++total_pieces_left[h.color];
		positions[h.color][h.piece].erase(find(positions[h.color][h.piece].begin(), positions[h.color][h.piece].end(), h));
	}

	vector<Hex> Game::valid_spawns(Color color)
	{
		static bool visited[GSIDE][GSIDE];

		assert(color != Color::NoColor);

		memset(visited, 0, sizeof(visited));
		vector<Hex> vs; // valid spawns
		queue<Hex> q;
		q.push(get_hex_with_color(color));
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

	vector<Hex> Game::valid_moves(Hex p)
	{
		assert(p.piece != Piece::NoPiece);
		assert(p.color != Color::NoColor);

		if (is_locked(p) or not bee_spawned[p.color]) {
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

	vector<Hex> Game::ant_valid_moves(Hex h0)
	{
		static bool visited[GSIDE][GSIDE];

		destroy(h0); // Temporally delete piece

		vector<Hex> v; // rechable hexs
		if (count_components() == 1) {
			memset(visited, 0, sizeof(visited));
			queue<Hex> q;
			for (Hex h_ : get_neighbours(h0)) { // Find BFS origin
				if (grid[h_].piece != Piece::NoPiece) { 
					q.push(h_);
					break;
				}
			}
			while (not q.empty()) {
				Hex h = q.front();
				q.pop();
				for (Hex p : get_neighbours(h)) {
					if (not is_outside(p) and not visited[p.x][p.y]
						and is_accessible(h, p) and grid[p].piece == Piece::NoPiece
						and has_neighbour(p))
					{
						visited[p.x][p.y] = true;
						v.push_back(p);
						q.push(p);
					}
				}
			}
		}

		spawn(h0.x, h0.y, h0.color, h0.piece); // Restore piece
		return v;
	}

	vector<Hex> Game::bee_valid_moves(Hex h0)
	{
		destroy(h0); // Temporally delete piece

		vector<Hex> v; // rechable hexs
		if (count_components() == 1) {
			for (Hex p : get_neighbours(h0)) {
				if (grid[p].piece == Piece::NoPiece and is_accessible(h0, p)
					and has_neighbour(p))
				{
					v.push_back(p);
				}
			}
		}

		spawn(h0.x, h0.y, h0.color, h0.piece); // Restore piece
		return v;
	}
	
	vector<Hex> Game::beetle_valid_moves(Hex h0)
	{
		destroy(h0); // Temporally delete piece

		vector<Hex> v; // rechable hexs
		if (count_components() == 1) {
			for (Hex p : get_neighbours(h0, true)) {
				if (grid[p].piece == Piece::NoPiece and is_accessible(h0, p)
					and has_neighbour(p)
					and (p.layer == 0 or (p.layer == 1 and grid[p.x][p.y][0].piece == Piece::NoPiece)))
				{
					v.push_back(p);
				}
			}
		}

		spawn(h0.x, h0.y, h0.color, h0.piece, h0.layer); // Restore piece
		return v;
	}
	
	vector<Hex> Game::grasshopper_valid_moves(Hex h0)
	{
		destroy(h0); // Temporally delete piece

		vector<Hex> v; // rechable hexs
		if (count_components() == 1) {
			for (int dir_idx = 0; dir_idx < 6; ++dir_idx) {
				Hex p = h0 + dirs[h0.x%2][dir_idx];
				if (not is_outside(p) and grid[p].piece != Piece::NoPiece) {
					while (not is_outside(p) and grid[p].piece != Piece::NoPiece) {
						p = p + dirs[p.x%2][dir_idx];
					}
					if (not is_outside(p) and grid[p].piece == Piece::NoPiece) {
						v.push_back(p);
					}
				}
			}
		}

		spawn(h0.x, h0.y, h0.color, h0.piece); // Restore piece
		return v;
	}
	
	vector<Hex> Game::spider_valid_moves(Hex h0)
	{
		static int dist[GSIDE][GSIDE];

		destroy(h0); // Temporally delete piece

		vector<Hex> v; // rechable hexs
		if (count_components() == 1) {
			for (Hex h_ : get_neighbours(h0)) { // Find BFS origin
				if (grid[h_].piece == Piece::NoPiece and has_neighbour(h_)) { 
					queue<Hex> q;
					for (int y = max(h0.y - 3, 0); y <= min(h0.y + 3, (int)GSIDE-1); ++y) { // initialize rechable points dist
						for (int x = max(h0.x - 3, 0); x <= min(h0.x + 3, (int)GSIDE-1); ++x) {
							dist[x][y] = IINF;
						}
					}
					q.push(h_);
					dist[h0.x][h0.y] = 0;
					dist[h_.x][h_.y] = 1;
					while (not q.empty()) {
						Hex h = q.front();
						q.pop();
						for (Hex p : get_neighbours(h)) {
							if (not is_outside(p) and dist[p.x][p.y] == IINF
								and is_accessible(h, p) and grid[p].piece == Piece::NoPiece
								and has_neighbour(p))
							{
								dist[p.x][p.y] = dist[h.x][h.y] + 1;
								if (dist[p.x][p.y] == 3) v.push_back(p);
								if (dist[p.x][p.y] < 3) q.push(p);
							}
						}
					}
				}
			}
		}

		spawn(h0.x, h0.y, h0.color, h0.piece); // Restore piece
		return v;
	}

	Hex Game::get_hex_with_color(Color color)
	{
		for (Piece piece : PIECES) {
			for (Hex h : positions[color][piece]) {
				if (h.layer == 0) return h;
			}
		}
		assert(false); // error
	}

	Hex Game::get_hex_with_any_piece()
	{
		for (Color color : COLORS) {
			for (Piece piece : PIECES) {
				for (Hex h : positions[color][piece]) {
					if (h.layer == 0) return h;
				}
			}
		}
		assert(false); // error
	}

	vector<Hex> Game::get_neighbours(Hex p, bool all_layers)
	{
		vector<Hex> v;
		for (const Hex& dir : dirs[p.x%2]) {
			Hex h_ = p + dir;
			for (int layer = int(all_layers); layer >= 0; --layer) {
				Hex h = Hex(layer, h_.x, h_.y);
				if (not is_outside(h)) {
					v.push_back(h);
					break;
				}
			}
		}
		return v;
	}

	int Game::count_components()
	{
		static bool visited[GSIDE][GSIDE];

		memset(visited, 0, sizeof(visited));
		Hex h0 = get_hex_with_any_piece();
		queue<Hex> q;
		for (Hex h_ : get_neighbours(h0)) { // Find BFS origin
			if (grid[h_].piece != Piece::NoPiece) { 
				q.push(h_);
				break;
			}
		}
		while (not q.empty()) {
			Hex h = q.front();
			q.pop();
			for (Hex p : get_neighbours(h)) {
				if (not is_outside(p) and not visited[p.x][p.y]
					and is_accessible(h, p) and grid[p].piece != Piece::NoPiece
					and has_neighbour(p))
				{
					visited[p.x][p.y] = true;
					q.push(p);
				}
			}
		}

		for (Color color : COLORS) {
			for (Piece piece : PIECES) {
				for (Hex h : positions[color][piece]) {
					if (not visited[h.x][h.y]) return 2; // optimization: only count 1 or 2 components
				}
			}
		}

		return 1;
	}
	
	Color Game::winner()
	{
		for (Color color : COLORS) {
			if (not positions[color][Piece::Bee].empty() and surrounding_cnt(positions[color][Piece::Bee][0]) == 6) {
				return (Color)!color; // enemy color
			}
		}
		return Color::NoColor;
	}

	int Game::surrounding_cnt(Hex h)
	{
		int cnt = 0;
		for (Hex p : get_neighbours(h)) {
			if (grid[p].piece != Piece::NoPiece) ++cnt;
		}
		return cnt;
	}

	unsigned long long Game::hash(long long depth)
	{
		unsigned long long H = 0;
		
		for (long long layer = 0; layer < 1; ++layer) {
			for (long long y = 0; y < GSIDE; ++y) {
				for (long long x = 0; x < GSIDE; ++x) {
					Hex h = grid[x][y][layer];
					H += powAmodB[layer*GSIDE*GSIDE+y*GSIDE+x] * ((h.piece + 1) * 3 + (h.color + 1)) % B;
					H %= B;
				}
			}
		}

		H += powAmodB[GSIDE*GSIDE*GSIDE-1] * depth % B;
		H %= B;

		return H;
	}

}

#endif