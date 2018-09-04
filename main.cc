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

const int WIDTH = 800, HEIGHT = 800;

SDL_Renderer* renderer = NULL;
SDL_Surface* hexgrid_img = NULL;
SDL_Texture* hexgrid_tex = NULL;
SDL_Surface* pieces_img[2][5]; // {color, piece id}
SDL_Texture* pieces_tex[2][5]; // {color, piece id}
int hex_w = -1, hex_h = -1;

void draw_circle(int cx, int cy, int r)
{
    int r2 = r*r;
    for (int x = -r; x <= r; ++x) {
        for (int y = -r; y <= r; ++y) {
            if (x*x + y*y <= r2) {
                SDL_RenderDrawPoint(renderer, cx+x, cy+y);
            }
        }
    }
}

void draw_hex(SDL_Texture* texture, int hexgrid_x, int hexgrid_y)
{
    int x = hexgrid_x * hex_w;
    int y = hexgrid_y * hex_h - (hexgrid_x & 1 ? hexgrid_img->h/2 : 0);
    SDL_Rect dstrect = { x, y, hexgrid_img->w, hexgrid_img->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
}

void draw_hexgrid(Game& game)
{
    assert(hex_w >= 0 and hex_h >= 0);

    for (int i = -1; i * hex_w <= WIDTH; ++i) {
        for (int j = -1; j * hex_h <= HEIGHT; ++j) {
            auto g = game.get_grid();
            if (game.is_outside(Hex(i, j)) or g[i][j][0].piece == Piece::NoPiece) {
                draw_hex(hexgrid_tex, i, j);
            }
            else {
                const Hex& h = g[i][j][0];
                draw_hex(pieces_tex[(int)h.color][(int)h.piece], i, j);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("HiveAI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    
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
    hexgrid_img = SDL_LoadBMP("img/hex/hexagon.bmp");
    hexgrid_tex = SDL_CreateTextureFromSurface(renderer, hexgrid_img);
    hex_w = 3 * hexgrid_img->w / 4;
    hex_h = hexgrid_img->h;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 5; ++j) {
            pieces_img[i][j] = SDL_LoadBMP(("img/hex/p" + to_string(i) + to_string(j) + ".bmp").c_str());
            pieces_tex[i][j] = SDL_CreateTextureFromSurface(renderer, pieces_img[i][j]);
        }
    }   

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Event windowEvent;
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (SDL_QUIT == windowEvent.type) {
                break;
            }
        }
        
        draw_hexgrid(game);

        // DEBUG: 
        for (Color c : {Color::Black, Color::White}) {
            for (Hex h : game.valid_spawns(c)) {
            // for (Hex h : game.valid_moves(Hex(GSIDE/2, GSIDE/2-1))) {
                int sx = h.x * hex_w;
                int sy = h.y * hex_h - (h.x&1 ? hexgrid_img->h/2 : 0);
                SDL_SetRenderDrawColor(
                    renderer, 
                    (c == 0 ? 255 : 0), 
                    0, 
                    (c == 1 ? 255 : 0), 
                    75
                );
                draw_circle(sx+hexgrid_img->w/2, sy+hexgrid_img->h/2, 10);
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