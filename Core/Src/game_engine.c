#include "game_engine.h"

void game_start(game_handle *handle) {
    handle->player_pos_x = handle->field_width / 2;
    handle->player_pos_y = handle->field_height / 2;
    handle->mines_count = handle->field_width * handle->field_height / 5 * 1;

    for(int i = 0; i < handle->field_width * handle->field_height; ++i) {
        handle->field[i] = 0b00000001;
    }

    for(uint8_t i = 0; i < handle->field_height; ++i) {
        for(uint8_t j = 0; j < handle->field_width; ++j) {
            //draw_picture(j * 16, i * 16, 16, 16,
                         //not_opened_cell, handle->graphics);
            draw_cell(j, i, handle);
        }
    }

    //draw_picture(handle->player_pos_x * 16, handle->player_pos_y * 16,
      //           16, 16, selected_cell, handle->graphics);
}

void game_player_move(int8_t x, int8_t y, game_handle *handle) {
    uint8_t old_x = handle->player_pos_x;
    uint8_t old_y = handle->player_pos_y;
    if(x) {
        //draw_picture(handle->player_pos_x * 16, handle->player_pos_y * 16,
         //        16, 16, not_opened_cell, handle->graphics);
        if(x < 0 && handle->player_pos_x == 0) {
            handle->player_pos_x = handle->field_width - 1;
        }
        else {
            handle->player_pos_x += x;
            if(handle->player_pos_x >= handle->field_width) {
                handle->player_pos_x = 0;
            }
        }

        //draw_picture(handle->player_pos_x * 16, handle->player_pos_y * 16,
        //         16, 16, selected_cell, handle->graphics);
    }

    if(y) {
        //draw_picture(handle->player_pos_x * 16, handle->player_pos_y * 16,
        //         16, 16, not_opened_cell, handle->graphics);
        if(y < 0 && handle->player_pos_y == 0) {
            handle->player_pos_y = handle->field_height - 1;
        }
        else {
            handle->player_pos_y += y;
            if(handle->player_pos_y >= handle->field_height) {
                handle->player_pos_y = 0;
            }
        }

        //draw_picture(handle->player_pos_x * 16, handle->player_pos_y * 16,
        //         16, 16, selected_cell, handle->graphics);
    }

    draw_cell(old_x, old_y, handle);
    draw_cell(handle->player_pos_x, handle->player_pos_y, handle);
}

void game_player_move_up(game_handle *handle) {
    game_player_move(0, -1, handle);
    return;
}

void game_player_move_down(game_handle *handle) {
    game_player_move(0, 1, handle);
    return;
}

void game_player_move_left(game_handle *handle) {
    game_player_move(-1, 0, handle);
    return;
}

void game_player_move_right(game_handle *handle) {
    game_player_move(1, 0, handle);
    return;
}

void game_player_put_flag(game_handle *handle) {
    if(!game_is_cell_opened(handle->player_pos_x, handle->player_pos_y, handle)) {
        if(game_is_cell_under_flag(handle->player_pos_x, handle->player_pos_y, handle)) {
            handle->field[handle->player_pos_x + handle->player_pos_y * handle->field_width] -= 2;
        }
        else {
            handle->field[handle->player_pos_x + handle->player_pos_y * handle->field_width] += 2;
        }
        draw_cell(handle->player_pos_x, handle->player_pos_y, handle);
    }
    return;
}

void game_player_open_cell(game_handle *handle) {
    if(
}

uint8_t game_get_cell_value(uint8_t x, uint8_t y, game_handle *handle) {
    return handle->field[y * handle->field_width + x] >> 4;
}

uint8_t game_is_cell_opened(uint8_t x, uint8_t y, game_handle *handle) {
    return (handle->field[y * handle->field_width + x] << 4) == 0;
}

uint8_t game_is_cell_under_flag(uint8_t x, uint8_t y, game_handle *handle) {
    return (handle->field[y * handle->field_width + x] << 4) == 0b00110000;
}

void draw_cell(uint8_t x, uint8_t y, game_handle *handle) {
    if(game_is_cell_opened(x, y, handle)) {
        switch(game_get_cell_value(x, y, handle)) {
            case 1:
                draw_picture(x * 16, y * 16, 16, 16, one, handle->graphics);
                break;
            case 2:
                draw_picture(x * 16, y * 16, 16, 16, two, handle->graphics);
                break;
            case 3:
                draw_picture(x * 16, y * 16, 16, 16, three, handle->graphics);
                break;
            case 4:
                draw_picture(x * 16, y * 16, 16, 16, four, handle->graphics);
                break;
            case 5:
                draw_picture(x * 16, y * 16, 16, 16, five, handle->graphics);
                break;
            case 6:
                draw_picture(x * 16, y * 16, 16, 16, six, handle->graphics);
                break;
            case 7:
                draw_picture(x * 16, y * 16, 16, 16, seven, handle->graphics);
                break;
            case 8:
                draw_picture(x * 16, y * 16, 16, 16, eight, handle->graphics);
                break;
            case 9:
                draw_picture(x * 16, y * 16, 16, 16, zero_cell, handle->graphics);
                break;
            case 10:
                draw_picture(x * 16, y * 16, 16, 16, bomb, handle->graphics);
                break;
        }
    }
    else {
        if(game_is_cell_under_flag(x, y, handle)) {
            draw_picture(x * 16, y * 16, 16, 16, flag, handle->graphics);
        }
        else {
            draw_picture(x * 16, y * 16, 16, 16, not_opened_cell, handle->graphics);
        }
    }

    if(handle->player_pos_x == x && handle->player_pos_y == y) {
        draw_picture_blend(x * 16, y * 16, 16, 16, selected_cell, handle->graphics);
    }
}
