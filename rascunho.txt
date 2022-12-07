/* 

int lastButton1State = LOW;  // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// https://docs.arduino.cc/built-in-examples/digital/Debounce
void t4(void){
  // read the state of the switch into a local variable:
  int reading = read_buttons(B11);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButton1State) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button1State) {
      button1State = reading;

      // only toggle the LED if the new button state is HIGH
      if (button1State == HIGH){
          if(game_rows_1[15]==2){
            score += 1;
            Serial.println(score);
            flag=false;
          }
        //flag = true;
        //Serial.println(flag);
      }
    }
  }
  
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButton1State = reading;
}*/
