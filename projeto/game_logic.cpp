#include "game_logic.h"


int game_state(){ // returns a random position 1 or 2 based on a random number generator
  
  unsigned int n_row=0;
  unsigned int randNumber=0;

  randNumber = random(100);
  
  if(randNumber<=50){
    n_row=1;
  }
  else{
    n_row=2;
  }
  return n_row;
}

int *game_row(int *game_rows, int *rows_aux){ // Updates current game state
  for(int i=15;i>=0;i--){                     // Copies the previous game state
    rows_aux[i]=game_rows[i];
  }
  game_rows[0]=game_state();
  for(int i=15;i>=0;i--){                     // Updates array to match what should be the new game state
    game_rows[i+1]=rows_aux[i];
  }
  return game_rows;
}
