#ifndef HIVE_CONSTANTS_H
#define HIVE_CONSTANTS_H

namespace Hive
{

	const int NPIECES = 24; // Number of pieces
	const int GSIDE = NPIECES; // Grid side size

	enum Piece { NoPiece = -1, Ant = 0, Bee = 1, Beetle = 2, Grasshopper = 3, Spider = 4 };
	enum Color { NoColor = -1, Black = 0, White = 1 };

}

#endif