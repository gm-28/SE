#include "game_logic.h"

int score=0;

int game_state(){
  
  unsigned int n_row=0;
  unsigned int randNumber=0;

  randNumber = random(50);
  if(randNumber<=25){
    n_row=1;
  }
  else{
    n_row=2;
  }
  return n_row;
}

int game_button(int row_aux){
  return (row_aux==1);
}

int *game_row(int *game_rows, int *rows_aux){
  for(int i=15;i>=0;i--){
    rows_aux[i]=game_rows[i];
  }
  game_rows[0]=game_state();
  for(int i=15;i>=0;i--){
    game_rows[i+1]=rows_aux[i];
  }
  return game_rows;
}

int game_score(){
    score+=1;
  Serial.print("\n");
  Serial.print(score);
  return score;
}
