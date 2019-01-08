#pragma GCC optimize("O3","unroll-loops","omit-frame-pointer","inline") // Optimization flags
#pragma GCC option("arch=native","tune=native","no-zero-upper") // Enable AVX
#pragma GCC target("avx")  //Enable AVX
#include <x86intrin.h> // AVX/SSE Extensions

#ifndef HIVE_CONSTANTS_H
#define HIVE_CONSTANTS_H

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <limits>

// DEBUG:
#define D(x) std::cout << #x << " = " << (x) << ", "

namespace Hive
{

	enum Piece { NoPiece = -1, Ant = 0, Bee = 1, Beetle = 2, Grasshopper = 3, Spider = 4 };
	enum Color { NoColor = -1, Black = 0, White = 1 };

	const long double EPS = 1e-7;
	const long double INF = std::numeric_limits<long double>::infinity();
	const int IINF = 0x7FFFFFFF;
	const int NPIECETYPES = 5;
	const int NPIECERPERPLAYER = 11;
	const int NPIECES = 2*NPIECERPERPLAYER; // Number of pieces
	const int GSIDE = 30; // Grid side size
	const int MAXDEPTH = 4;
	const Color player_color = Color::White;
	const Color ia_color = Color::Black;
	const std::array<Color,2> COLORS = {Color::Black, Color::White};
	const std::array<Piece,NPIECETYPES> PIECES = { Ant, Bee, Beetle, Grasshopper, Spider };
	const std::array<int,NPIECETYPES> PIECEVAL = { 6, 10, 4, 2, 2 };
}

#endif