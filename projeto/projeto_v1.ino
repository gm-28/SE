/* Define shift register pins used for seven segment display */
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "buttons.h"
#include "game_logic.h"
#include "displays.h"

// defines won't change. Used here to set pin numbers
#define B11 2 // the number of the pushbutton 1 pin of player 1
#define B12 3 // the number of the pushbutton 2 pin of player 1
#define B21 4 // the number of the pushbutton 1 pin of player 2
#define B22 5 // the number of the pushbutton 2 pin of player 2

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd1(0x20); // DAT -> A4 CLK -> A5
Adafruit_LiquidCrystal lcd2(0x21); // DAT -> A4 CLK -> A5

int button1State;            // the current reading from the input pin
int lastButton1State = LOW;  // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

int score = 0;
bool flag = false;

byte customChar[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111}; // display character

int game_rows_1[16]={NULL}; 
int game_rows_2[16]={NULL};
int rows_aux_1[16];
int rows_aux_2[16];

//timer1 interrupt
ISR(TIMER1_COMPA_vect){
  Sched_Schedule();
}

void t1(void) {
  //Serial.print(" A");
  lcd_refresh(&lcd1, game_row(game_rows_1,rows_aux_1));
  lcd_refresh(&lcd2, game_row(game_rows_2,rows_aux_2));
} 

void t2(void) {
  //Serial.print(" B");
  //lcd_refresh(&lcd2, game_row(game_rows_2,rows_aux_2));
  if(flag){
    score +=1;
    flag = false;
    Serial.println(score);
  }
}

void t3(void) {
  button1State = read_buttons(B11); // read the state of the pushbutton value
  //buttonState_2 = read_buttons(B12); // read the state of the pushbutton value
  
  if (button1State == HIGH){
    if(game_rows_1[15]==2){
      flag=true;
    }
  }
}

/*
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

void setup() {
  //INIT SCHEDULER

  Sched_Init();
  Sched_AddT(t1, 0, 500);
  Sched_AddT(t2, 0, 500);
  Sched_AddT(t3, 0, 50);
  //Sched_AddT(t5, 0, 500);
  //ched_AddT(t4, 0, 50);

  
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
 
  //OCR1A  = 31250; // compare match register 16MHz/256/2Hz
  OCR1A  = 62.5; // compare match register 16MHz/256/1kHz
  //OCR1A = 31;    // compare match register 16MHz/256/2kHz
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts

  set_up_button(B11);
  set_up_button(B12);
  set_up_button(B21);
  set_up_button(B22);

  lcd1.clear();
  lcd1.begin(16, 2);  // set up the lcd1's number of columns and rows
  lcd1.createChar(1,customChar);
  
  lcd2.clear();
  lcd2.begin(16, 2);  // set up the LCD's number of columns and rows
  lcd2.createChar(1,customChar);

  Serial.begin(9600);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  Sched_Dispatch();

}

//KERNEKL CODE
typedef struct{
  int period; /* period in ticks */
  int delay; /* ticks until next activation */
  void (*func)(void); /* function pointer */
  int exec; /* activation counter */
} Sched_Task_t;

#define NT 20
Sched_Task_t Tasks[NT];

int Sched_Init(void)
{
  for(int x=0; x<NT; x++)
    Tasks[x].func = 0;
}

int Sched_AddT(void (*f)(void),int d, int p)
{
  for(int x=0; x<NT; x++)
  if (!Tasks[x].func) 
  {
    Tasks[x].period = p;
    Tasks[x].delay = d;
    Tasks[x].exec = 0;
    Tasks[x].func = f;
    return x;
  }
  return -1;
}

void Sched_Schedule(void){
  for(int x=0; x<NT; x++){
    if(Tasks[x].func){
      if(Tasks[x].delay){
        Tasks[x].delay--;
      } 
      else{
        /* Schedule Task */
        Tasks[x].exec++;
        Tasks[x].delay = Tasks[x].period-1;
      }
    }
  }
}

void Sched_Dispatch(void){
  for(int x=0; x<NT; x++) {
    if((Tasks[x].func)&&(Tasks[x].exec)) {
      Tasks[x].exec=0;
      Tasks[x].func();
      /* Delete task
      * if one-shot */
      if(!Tasks[x].period) Tasks[x].func = 0;
      return; //TURNS TO FIXED PRIORITIE
    }
  }
}