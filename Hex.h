    #ifndef HIVE_HEX_H
#define HIVE_HEX_H

#include "Constants.h"
#include <iostream>
#include <cassert>

namespace Hive 
{

    class Hex // Hexagon
    {
        public:
            friend std::ostream& operator<<(std::ostream& os, const Hex& h);
            Hex() : layer(0), color(Color::NoColor), x(0), y(0), piece(Piece::NoPiece) {};
            Hex(int _x, int _y) : layer(0), color(Color::NoColor), x(_x), y(_y), piece(Piece::NoPiece) {};
            Hex(int _layer, Color _color, int _x, int _y) { 
                setup(_layer, _color, _x, _y, NoPiece); 
            };
            Hex(int _layer, Color _color, int _x, int _y, Piece _piece) {
                setup(_layer, _color, _x, _y, _piece); 
            };
            Hex operator+(Hex h) const;
            // layer : [0, 1]
            // color : [-1, 1]
            // x : [0, GSIDE]
            // y : [0, GSIDE]
            int layer, x, y;
            Color color;
            Piece piece;
        private:
            void setup(int _layer, Color _color, int _x, int _y, Piece _piece);
    };

    std::ostream& operator<<(std::ostream& os, const Hex& h)
    {  
        os << '{' << h.layer << ',' << h.color << ',' << h.x << ',' << h.y << ',' << h.piece << '}';
        return os;  
    }  

    void Hex::setup(int _layer, Color _color, int _x, int _y, Piece _piece)
    {
        if (_piece != Piece::NoPiece) {
            // Error if real piece is outside grid bounds or has invalid color
            assert(
                _layer >= 0 and _layer <= 1
                and color != Color::NoColor
                and _x >= 0 and _x < GSIDE
                and _y >= 0 and _y < GSIDE
            );
        }

        layer = _layer;
        color = _color;
        x = _x;
        y = _y;
        piece = _piece;
    }

    Hex Hex::operator+(Hex h) const
    {
        return Hex(layer, color, x + h.x, y + h.y, piece);
    }

}

#endif