#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <stdlib.h>
#include "gdi.h"
#include "game_symbols.h"

typedef struct {
    uint8_t field_width;
    uint8_t field_height;
    uint8_t *field;
    gdi_handle *graphics;
    uint8_t player_pos_x;
    uint8_t player_pos_y;
    uint16_t mines_count;
    uint16_t openings;
} game_handle;

uint8_t game_get_cell_value(uint8_t x, uint8_t y, game_handle *handle);
uint8_t game_is_cell_opened(uint8_t x, uint8_t y, game_handle *handle);
uint8_t game_is_cell_under_flag(uint8_t x, uint8_t y, game_handle *handle);

void draw_cell(uint8_t x, uint8_t y, game_handle *handle);

void game_start(game_handle *handle);
void game_player_move(int8_t x, int8_t y, game_handle *handle);
void game_player_move_up(game_handle *handle);
void game_player_move_down(game_handle *handle);
void game_player_move_left(game_handle *handle);
void game_player_move_right(game_handle *handle);
void game_player_put_flag(game_handle *handle);
void game_player_open_cell(game_handle *handle);

#endif