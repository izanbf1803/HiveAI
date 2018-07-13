#ifndef HIVE_HEXGRID_H
#define HIVE_HEXGRID_H

#include "Constants.h"
#include "Hex.h"
#include <array>

using std::array;

namespace Hive 
{

    class HexGrid // Hexag Grid
    {
        public:
            array<array<array<Hex,NPIECES>,NPIECES>,2> grid; // layer, y, x
            HexGrid() {}
            array<array<Hex,NPIECES>,NPIECES>& operator[](int layer);
        private:
    };

    array<array<Hex,NPIECES>,NPIECES>& HexGrid::operator[](int layer)
    {
        assert(layer >= 0 and layer <= 1);

        return grid[layer];
    }

}

#endif