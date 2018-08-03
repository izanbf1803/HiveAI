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
const int offset_x = 16, offset_y = 14;

SDL_Renderer* renderer = NULL;
SDL_Surface* hexgrid_img = NULL;
SDL_Texture* hexgrid_tex = NULL;
int hex_w = -1, hex_h = -1;

void draw_circle(int cx, int cy, int r)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
    int r2 = r*r;
    for (int x = -r; x <= r; ++x) {
        for (int y = -r; y <= r; ++y) {
            if (x*x + y*y <= r2) {
                SDL_RenderDrawPoint(renderer, cx+x, cy+y);
            }
        }
    }
}

void draw_hexgrid()
{
    assert(hex_w >= 0 and hex_h >= 0);

    for (int i = -1; i * hex_w <= WIDTH; ++i) {
        for (int j = -1; j * hex_h <= HEIGHT; ++j) {
            int x = i * hex_w;
            int y = j * hex_h - (i&1 ? hexgrid_img->h/2 : 0);
            SDL_Rect dstrect = { x, y, hexgrid_img->w, hexgrid_img->h };
            SDL_RenderCopy(renderer, hexgrid_tex, NULL, &dstrect);
        }
    }
}

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


    renderer = SDL_CreateRenderer(window, -1, 0);
    hexgrid_img = SDL_LoadBMP("img/hexagon.bmp");
    hexgrid_tex = SDL_CreateTextureFromSurface(renderer, hexgrid_img);
    hex_w = 24;
    hex_h = hexgrid_img->h;

    SDL_Event windowEvent;
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (SDL_QUIT == windowEvent.type) {
                break;
            }
        }
        
        draw_hexgrid();
        for (int i = 0; i * hex_w < WIDTH; ++i) {
            for (int j = 0; j * hex_h < HEIGHT; ++j) {
                int x = i * hex_w;
                int y = j * hex_h - (i&1 ? hexgrid_img->h/2 : 0);
                draw_circle(x+offset_x, y+offset_y, 10);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }   

    SDL_DestroyTexture(hexgrid_tex);
    SDL_FreeSurface(hexgrid_img);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}