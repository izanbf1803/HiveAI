#ifndef HIVE_HEXGRID_H
#define HIVE_HEXGRID_H

#include "Hex.h"

namespace Hive 
{
    using std::array;

    class HexGrid // Hexag Grid
    {
        public:
            HexGrid();
            inline array<array<Hex,2>,GSIDE>& operator[](int i) { return grid[i]; };
            Hex& operator[](Hex h) { return grid[h.x][h.y][h.layer]; };
            array<array<array<Hex,2>,GSIDE>,GSIDE> grid; // x, y, layer
        private:
    };

    HexGrid::HexGrid() 
    {
        for (int y = 0; y < GSIDE; ++y) {
    		for (int x = 0; x < GSIDE; ++x) {
    		    grid[x][y][0] = Hex(0, x, y);
                grid[x][y][1] = Hex(1, x, y);
            }
        }
    }

}

#endif