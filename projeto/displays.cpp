#include "displays.h"
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

void lcd_refresh(Adafruit_LiquidCrystal *l, int *game_rows){
  l->clear();
  
  for(int i=0;i<=15;i++){
    if(game_rows[i]!=NULL){
      if(game_rows[i] == 2){
        l->setCursor(i, 0);
        l->write(1);
      }
      else if(game_rows[i] == 1){
        l->setCursor(i, 1);
        l->write(1);
      }
    }
  }
}