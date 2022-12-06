#include "buttons.h"

void set_up_button(byte pin){
  pinMode(pin, INPUT);  // initialize the pushbutton pin as an input
}

int read_buttons(byte pin){
  return(digitalRead(pin));  // read the state of the pushbutton value
}