#pragma GCC optimize("Ofast","unroll-loops","omit-frame-pointer","inline") // Optimization flags
#pragma GCC option("arch=native","tune=native","no-zero-upper") // Enable AVX
#pragma GCC target("avx")  // Enable AVX
// #include <x86intrin.h> // AVX/SSE Extensions

#ifndef HIVE_CONSTANTS_H
#define HIVE_CONSTANTS_H

#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <limits>

// DEBUG:
#define D(x) std::cout << #x << " = " << (x) << ", "

#define USE_MCTS 0

namespace Hive
{

	enum Piece { NoPiece = -1, Ant = 0, Bee = 1, Beetle = 2, Grasshopper = 3, Spider = 4 };
	enum Color { NoColor = -1, Black = 0, White = 1 };

	const bool DEBUG = false;
	const int IINF = 0x7FFFFFFF;
	const long long LINF = 0x7FFFFFFFFFFFFFFF;
	const long double INF = 1e300;
	const long double EPS = 1e-7;
	const int NPIECETYPES = 5;
	const int NPIECERPERPLAYER = 11;
	const int NPIECES = 2*NPIECERPERPLAYER; // Number of pieces
	const long long GSIDE = 30; // Grid side size
	// const int MAXDEPTH = 4;
	const int TLE = 5000; // milliseconds max time for ia turn
	const int TT_size = 16384;
	const Color player_color = Color::White;
	const Color ia_color = Color::Black;
	const std::array<Color,2> COLORS = {Color::Black, Color::White};
	const std::array<Piece,NPIECETYPES> PIECES = { Ant, Bee, Beetle, Grasshopper, Spider };
	const std::array<int,NPIECETYPES> PIECEVAL = { 3, 5, 2, 2, 1 };
	long long pow10[19];
	unsigned long long powAmodB[GSIDE*GSIDE*GSIDE];
	const unsigned long long A = 2999999929;
	const unsigned long long B = 3333323333;
	const long double C = 1.4142135623730951; // sqrt(2)

	void precompute_global_variables()
	{
		pow10[0] = 1;
		for (int i = 1; i < 19; ++i) {
			pow10[i] = pow10[i-1] * 10;
		}

		powAmodB[0] = 1;
		for (int i = 1; i < GSIDE*GSIDE*GSIDE; ++i) {
			powAmodB[i] = powAmodB[i-1] * A % B;
		}
	}
}

#endif