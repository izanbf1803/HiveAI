#ifndef HIVE_HEX_H
#define HIVE_HEX_H

#include "Constants.h"

namespace Hive 
{

    class Hex // Hexagon
    {
        public:
            friend std::ostream& operator<<(std::ostream& os, const Hex& h);
            Hex() : layer(-1), color(Color::NoColor), x(-1), y(-1), piece(Piece::NoPiece) {};
            Hex(int _x, int _y) : layer(-1), color(Color::NoColor), x(_x), y(_y), piece(Piece::NoPiece) {};
            Hex(int _layer, int _x, int _y) : layer(_layer), color(Color::NoColor), x(_x), y(_y), piece(Piece::NoPiece) {};
            Hex(int _layer, Color _color, int _x, int _y, Piece _piece) : layer(_layer), color(_color), x(_x), y(_y), piece(_piece) {};
            Hex operator+(const Hex& h) const;
            bool operator==(const Hex& h) const;
            inline long long id() const;
            // layer : [0, 1]
            // color : [-1, 1]
            // x : [0, GSIDE-1]
            // y : [0, GSIDE-1]
            int layer, x, y;
            Color color;
            Piece piece;
        private:
    };

    std::ostream& operator<<(std::ostream& os, const Hex& h)
    {  
        os << '{' << h.layer << ',' << h.color << ',' << h.x << ',' << h.y << ',' << h.piece << '}';
        return os;  
    }  

    Hex Hex::operator+(const Hex& h) const
    {
        return Hex(layer, color, x + h.x, y + h.y, piece);
    }

    bool Hex::operator==(const Hex& h) const
    {
        return layer == h.layer
            && x == h.x && y == h.y 
            && color == h.color 
            && piece == h.piece;
    }

    inline long long Hex::id() const
    {
        return (((long long)x)<<12LL) + (((long long)y)<<6LL) + (long long)layer;
    }

}

#endif