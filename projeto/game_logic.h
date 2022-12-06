#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <Arduino.h>

int game_state();
int game_button(int row_aux);
int *game_row(int *game_rows, int *rows_aux);
int game_score();

#endif