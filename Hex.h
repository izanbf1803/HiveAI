#ifndef HIVE_HEX_H
#define HIVE_HEX_H

#include <iostream>
#include <cassert>

namespace Hive 
{

    class Hex // Hexagon
    {
        public:
            int layer, x, y;
            Piece piece;
            friend std::ostream& operator<<(std::ostream& os, const Hex& h);
            Hex() : layer(0), x(0), y(0), piece(None) {};
            Hex(int _layer, int _x, int _y) { setup(_layer, _x, _y, None); };
            Hex(int _layer, int _x, int _y, Piece _piece) { setup(_layer, _x, _y, _piece); };
        private:
            void setup(int _layer, int _x, int _y, Piece _piece);
    };

    std::ostream& operator<<(std::ostream& os, const Hex& h)
    {  
        os << '{' << h.layer << ',' << h.x << ',' << h.y << ',' << h.piece << '}';
        return os;  
    }  

    void Hex::setup(int _layer, int _x, int _y, Piece _piece)
    {
        assert(
            _layer >= 0 and _layer <= 1
            and _x >= 0 and _x < NPIECES
            and _y >= 0 and _y < NPIECES
        );

        layer = _layer;
        x = _x;
        y = _y;
        piece = _piece;
    }

}

#endif