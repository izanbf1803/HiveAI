// #include "Hive.h"
// #include <iostream>
// using namespace Hive;
// using namespace std;

// int main(int argc, char** argv)
// {
//     Game game(Piece::Ant);
//     auto g = game.get_grid();

//     for (int y = 0; y < GSIDE; ++y) {
//     	for (int x = 0; x < GSIDE; ++x) {
//     		cout << g[x][y][0] << ' ';
//     	}
//     	cout << endl;
//     }

//     cout << endl << endl;
//     cout << Hex(1, 1) + Hex(-1, 2) << endl;

//     cout << endl << endl;
//     for (Hex h : game.valid_spawns((Color)1)) {
//     	cout << "::" << h << endl;
//     }

//     return 0;
// }

#include <SDL2/SDL.h>
#include "Hive.h"
#include <iostream>
using namespace Hive;
using namespace std;

const int WIDTH = 800, HEIGHT = 600;

int main( int argc, char *argv[] )
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Hello SDL World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    
    // Check that the window was successfully created
    if (NULL == window) {
        // In the case that the window could not be made...
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_Event windowEvent;
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
    	cout << "::" << h << endl;
    }
    
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (SDL_QUIT == windowEvent.type) {
                break;
            }
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}