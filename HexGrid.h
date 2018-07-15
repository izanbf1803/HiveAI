#ifndef HIVE_HEXGRID_H
#define HIVE_HEXGRID_H

#include "Hex.h"
#include <array>

using std::array;

namespace Hive 
{

    class HexGrid // Hexag Grid
    {
        public:
            HexGrid() {}
            inline array<array<Hex,2>,NPIECES>& operator[](int i) { 
            	return grid[i]; 
            };
            array<array<array<Hex,2>,NPIECES>,NPIECES> grid; // x, y, layer
        private:
    };

}

#endif