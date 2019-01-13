#include <SDL2/SDL.h>
#include "Hive.h"
#include "Constants.h"
#include "AI.h"
#include "Minimax.h"
#include "MCTS.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
using namespace Hive;
using namespace std;

const int WIDTH = 700, HEIGHT = 700;

SDL_Renderer* renderer = NULL;
SDL_Surface* hexgrid_img = NULL;
SDL_Texture* hexgrid_tex = NULL;
SDL_Surface* pieces_img[2][5]; // {color, piece id}
SDL_Texture* pieces_tex[2][5]; // {color, piece id}
int hex_w = -1, hex_h = -1;
int mouse_x, mouse_y;
int selected_piece = 0;
Hex selected_hex = Hex();

inline int grid_to_screen_x(int x) { return x * hex_w; }
inline int grid_to_screen_y(int grid_x, int y) { return y * hex_h - (grid_x & 1 ? hexgrid_img->h/2 : 0); }
inline int screen_to_grid_x(int x) { return x / hex_w; }
inline int screen_to_grid_y(int grid_x, int y) { return (y + (grid_x & 1 ? hexgrid_img->h/2 : 0)) / hex_h; }

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
    assert(hex_w >= 0 && hex_h >= 0);
    // TODO: implement multiple layers
    for (int layer = 0; layer < 2; ++layer) {
        for (int i = -1; i * hex_w <= WIDTH; ++i) {
            for (int j = -1; j * hex_h <= HEIGHT; ++j) {
                if (game.is_outside(Hex(layer, i, j)) || game.grid[i][j][layer].piece == Piece::NoPiece) {
                    if (layer == 0) draw_hex(hexgrid_tex, i, j);
                }
                else {
                    const Hex& h = game.grid[i][j][layer];
                    SDL_Texture* texture = pieces_tex[h.color][h.piece];
                    if (h == selected_hex) {
                        SDL_SetTextureAlphaMod(texture, 75);
                    }
                    draw_hex(texture, i, j);
                    if (h == selected_hex) {
                        // Restore
                        SDL_SetTextureAlphaMod(texture, 255);
                    }
                }
            }
        }
    }
}

void draw_piece(int piece)
{  
    int x = mouse_x - hexgrid_img->w/2;
    int y = mouse_y - hexgrid_img->h/2;
    SDL_Rect dstrect = { x, y, hexgrid_img->w, hexgrid_img->h };
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, pieces_img[player_color][piece]);
    SDL_SetTextureAlphaMod(tex, 175);
    SDL_RenderCopy(renderer, tex, NULL, &dstrect);
}

bool select_piece(Game& game, int x, int y)
{
    if (selected_hex.piece != Piece::NoPiece) {
        selected_hex = Hex();
        return true;
    }
    else {
        for (int layer = 1; layer >= 0; --layer) {
            Hex h = game.grid[x][y][layer];
            // if (h.color == player_color) {
            if (h.piece != Piece::NoPiece) {
                selected_hex = h;
                return true;
            }
        }
    }
    return false;
}

void finish_game(Game& game, Color winner)
{
    vector<Hex> to_destroy;
    for (Piece piece : PIECES) {
        for (Hex h : game.positions[(Color)!winner][piece]) {
            to_destroy.push_back(h);
        }
    }
    for (Hex h : to_destroy) {
        game.destroy(h);
    }
}   

int main(int argc, char *argv[])
{
    srand(time(0)); // required to work with random numbers
    precompute_global_variables(); // NEVER remove this
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("HiveAI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    
    // Check that the window was successfully created
    if (NULL == window) {
        // In the case that the window could not be made...
        if (DEBUG) cout << "Could not create window: " << SDL_GetError() << endl;
        return 1;
    }
    
    Game game(Piece::Spider);

    renderer = SDL_CreateRenderer(window, -1, 0);
    hexgrid_img = SDL_LoadBMP("img/hex/hexagon.bmp");
    hexgrid_tex = SDL_CreateTextureFromSurface(renderer, hexgrid_img);
    float hexgrid_scale = 1.0 * HEIGHT / (NPIECES * hexgrid_img->h);
    hexgrid_img->w *= hexgrid_scale;
    hexgrid_img->h *= hexgrid_scale;
    hex_w = 3 * hexgrid_img->w / 4;
    hex_h = hexgrid_img->h;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 5; ++j) {
            pieces_img[i][j] = SDL_LoadBMP(("img/hex/p" + to_string(i) + to_string(j) + ".bmp").c_str());
            pieces_tex[i][j] = SDL_CreateTextureFromSurface(renderer, pieces_img[i][j]);
        }
    }   

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

#if USE_MCTS
    MCTS::Node* mcts = new MCTS::Node;
    mcts->play = AI::play_info_null();
    mcts->color = ia_color;
#endif

    Color winner = Color::NoColor;
    SDL_Event event;
    while (true) {
        if (winner == Color::NoColor) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            if (SDL_PollEvent(&event)) {
                AI::PlayInfo player_play = AI::play_info_null();
                int x = screen_to_grid_x(mouse_x);
                int y = screen_to_grid_y(x, mouse_y);
                if (SDL_QUIT == event.type) {
                    break;
                }
                else if (SDL_MOUSEBUTTONUP == event.type) {
                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT: {
                            bool valid = false;
                            if (selected_hex.color == Color::NoColor) {
                                valid = game.put_piece(x, y, player_color, (Piece)selected_piece);
                                if (valid) player_play = AI::play_info_put(0, Hex(0, x, y), (Piece)selected_piece);
                            }
                            else {
                                // int layer = (selected_hex.piece == Piece::Beetle && game.grid[x][y][0].piece != Piece::NoPiece ? 1 : 0);
                                // valid = game.move_piece(x, y, selected_hex, layer);
                                if (selected_hex.color == player_color) {
                                    valid = game.move_piece(x, y, selected_hex);
                                    if (valid) player_play = AI::play_info_move(0, selected_hex, Hex(0, x, y), selected_hex.piece);
                                    if (!valid && selected_hex.piece == Piece::Beetle) {
                                        valid = game.move_piece(x, y, selected_hex, 1);
                                        if (valid) player_play = AI::play_info_move(0, selected_hex, Hex(1, x, y), selected_hex.piece);
                                    }
                                }

                                if (valid) {
                                    winner = game.winner();
                                    if (winner != Color::NoColor) finish_game(game, winner);
                                    selected_hex = Hex();
                                }
                            }
                            if (DEBUG) cout << "PUT_event " << valid << endl;
                            if (valid && winner == Color::NoColor) {
                                if (DEBUG) cout << "IA turn:" << endl;

#if USE_MCTS
                                mcts = new MCTS::Node;
                                mcts->play = AI::play_info_null();
                                mcts->color = ia_color;


                                // for (MCTS::Node* child : mcts->childs) {
                                //     if (child->play == player_play) {
                                //         mcts = child;
                                //         break;
                                //     }
                                // }
                                // mcts->parent = NULL;
                                // mcts->play = AI::play_info_null();
                                // D(mcts->color) << endl;


                                mcts = mcts->play_hive(game);
#else
                                Minimax::play_hive(game);
#endif
                                

                                if (DEBUG) cout << "IA turn - END" << endl;
                                winner = game.winner();
                                if (winner != Color::NoColor) finish_game(game, winner);
                            }
                            break;
                        }
                        case SDL_BUTTON_RIGHT: {
                            bool valid = select_piece(game, x, y);
                            if (DEBUG) cout << "PICK_event " << valid << endl;
                            break;
                        }
                        default: break;
                    }
                    if (DEBUG) D(selected_hex) <<  endl;
                }
                else if (SDL_TEXTINPUT == event.type) {
                    char c = tolower(event.text.text[0]);
                    if (c >= '1' && c <= '5') {
                        selected_piece = c - '0' - 1;
                    }

                    // DEBUG:
                    if (DEBUG && c == 'd') {
                        cout << "--> " << x << " " << y << " -  l0: " << game.grid[x][y][0] << "  " << "l1: " <<  game.grid[x][y][1] << endl;
                    }
                }
            }
        }
        else {
            SDL_PollEvent(&event); // ignore input
        }
        
        draw_hexgrid(game);

        if (winner == Color::NoColor) {
            if (selected_hex.color == Color::NoColor) {
                if (game.pieces_left[player_color][selected_piece] > 0) draw_piece(selected_piece);
            }
            else {
                draw_piece(selected_hex.piece);
            }
        }

        for (Color c : COLORS) {
            if (winner == Color::NoColor || c == winner) {
                vector<Hex> locations;
                if (winner != Color::NoColor || selected_hex.piece == Piece::NoPiece) {
                    locations = game.valid_spawns(c);
                }
                if (winner == Color::NoColor && selected_hex.piece != Piece::NoPiece && selected_hex.color == c) {
                    locations = game.valid_moves(game.grid[selected_hex]);
                }
                for (Hex h : locations) {
                    int sx = h.x * hex_w;
                    int sy = h.y * hex_h - (h.x&1 ? hexgrid_img->h/2 : 0);
                    SDL_SetRenderDrawColor(
                        renderer, 
                        (c == 0 ? 255 : 0), 
                        0, 
                        (c == 1 ? 255 : 0), 
                        75
                    );
                    draw_circle(sx+hexgrid_img->w/2, sy+hexgrid_img->h/2, hexgrid_img->h/3);
                    // Restore:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                }
            }
        }

        SDL_Delay(1);
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