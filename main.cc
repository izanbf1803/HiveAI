#include "Hive.h"
#include <iostream>
using namespace Hive;
using namespace std;

int main(int argc, char** argv)
{
    Game game(Piece::Ant);
    auto g = game.get_grid();

    for (int y = 0; y < GSIDE; ++y) {
    	for (int x = 0; x < GSIDE; ++x) {
    		cout << g[x][y][0] << ' ';
    	}
    	cout << endl;
    }	

    cout << endl << endl;
    cout << Hex(1, 1) + Hex(-1, 2) << endl;

    cout << endl << endl;
    for (Hex h : game.valid_spawns((Color)1)) {
    	cout << h << endl;
    }

    return 0;
}