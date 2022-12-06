#ifndef DISPLAYS_H
#define DISPLAYS_H

#include "Adafruit_LiquidCrystal.h"
#include <Arduino.h>

void lcd_refresh(Adafruit_LiquidCrystal *l, int *game_rows);

#endif