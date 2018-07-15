#include "Hive.h"
#include <iostream>
using namespace Hive;
using namespace std;

int main(int argc, char** argv)
{
    Game game(Piece::Ant);
    auto g = game.get_grid();

    for (int y = 0; y < NPIECES; ++y) {
    	for (int x = 0; x < NPIECES; ++x) {
    		cout << g[x][y][0] << ' ';
    	}
    	cout << endl;
    }	

    return 0;
}