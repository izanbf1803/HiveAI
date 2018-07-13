#include "HexGrid.h"
#include <iostream>
using namespace Hive;
using namespace std;

int main(int argc, char** argv)
{
    HexGrid g;
    for (int i = 0; i < g[0].size(); ++i) {
        for (int j = 0; j < g[0][i].size(); ++j) {
            g[0][i][j] = Hex(1, 2, 3, (Piece)4);
            cout << g[0][i][j] << ' ';
        }
        cout << endl;
    }

    return 0;
}