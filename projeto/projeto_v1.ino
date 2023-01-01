#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "buttons.h"
#include "game_logic.h"
#include "displays.h"

// defines won't change. Used here to set pin numbers
#define B11 2      // the number of the pushbutton 1 pin of player 1
#define B12 3      // the number of the pushbutton 2 pin of player 1
#define B21 5      // the number of the pushbutton 1 pin of player 2
#define B22 6      // the number of the pushbutton 2 pin of player 2
#define B_RESET 12 // the number of the reset pin

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd1(0x20); // DAT -> A4 CLK -> A5
Adafruit_LiquidCrystal lcd2(0x21); // DAT -> A4 CLK -> A5

int buttonState_11; // value of the pushbutton 1 of Player 1
int buttonState_12; // value of the pushbutton 2 of Player 1
int buttonState_21; // value of the pushbutton 1 of Player 2         
int buttonState_22; // value of the pushbutton 2 of Player 2  

int p1_score = 0;  // score of Player 1
int p2_score = 0;  // score of Player 2
int p1_life = 3;   // nº of lives of Player 1
int p2_life = 3;   // nº of lives of Player 2

bool global_flag_1 = false;
bool global_flag_2 = false;
bool global_flag_3 = false;

bool flag_11 = false;
bool flag_12 = false;
bool flag_21 = false;
bool flag_22 = false;
bool dead_11 = false;
bool dead_12 = false;
bool dead_21 = false;
bool dead_22 = false;

String reset = "";  // value of the Input string inserted in the Serial Monitor

byte customChar[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111}; // game character displayed on the LCDs

int game_rows_1[16]={NULL}; 
int game_rows_2[16]={NULL};
int rows_aux_1[16];
int rows_aux_2[16];

//timer1 interrupt
ISR(TIMER1_COMPA_vect){
  Sched_Schedule();
}

void t1(void) {

  if(p1_life==0 && p2_life==0 && global_flag_3==false){   // both Players lost all their lives
    Serial.println("Press R to reset");                   // displays on the Serial Monitor the message to reset the game
    global_flag_3=true;
    memset(game_rows_1, 0, sizeof(game_rows_1));          // clears array...
    memset(rows_aux_1, 0, sizeof(rows_aux_1));            // clears array...
    memset(game_rows_2, 0, sizeof(game_rows_2));          // clears array...
    memset(rows_aux_2, 0, sizeof(rows_aux_2));            // clears array...
  }

  if(p1_life!=0){
    lcd_refresh(&lcd1, game_row(game_rows_1,rows_aux_1)); // refreshes the screen of LCD of Player 1
  }
  else{
    if(!global_flag_1){
      lcd1.clear();                                       // clears screen of the LCD of Player 1
      global_flag_1=true;
    }
    
    lcd1.setCursor(2,0);
    lcd1.print("Game Over :(");                           // indicates in the top row of the LCD of Player 1 that he lost all his lives 
    lcd1.setCursor(4,1);

    if(p1_score<10){
      lcd1.print(String("Score  ") + String(p1_score));   // displays on the LCD the score that Player 1 achieved if he scored less than 10 in the bottom row
    }
    else{
      lcd1.print(String("Score ") + String(p1_score));    // displays on the LCD the score that Player 1 achieved if he scored more than 10 in the bottom row
    }
  }

  if(p2_life!=0){
    lcd_refresh(&lcd2, game_row(game_rows_2,rows_aux_2)); // refreshes the screen of LCD of Player 2
  }
  else{
    if(!global_flag_2){
      lcd2.clear();                                       // clears screen of the LCD of Player 2
      global_flag_2=true;
    }
    lcd2.setCursor(2,0);
    lcd2.print("Game Over :(");                           // indicates in the top row of the LCD of Player 2 that he lost all his lives 
    lcd2.setCursor(4,1);
    
    if(p2_score<10){                                      
      lcd2.print(String("Score  ") + String(p2_score));   // displays on the LCD the score that Player 2 achieved if he scored less than 10 in the bottom row
    }
    else{
      lcd2.print(String("Score ") + String(p2_score));    // displays on the LCD the score that Player 2 achieved if he scored more than 10 in the bottom row
    }
  }
} 

void t2(void) {
  
  if(p1_life!=0){
    if(flag_11){
      p1_score+=1;              // adds 1 point to Player 1 score
      flag_11 = false;
      Serial.print("P11 ");     // displays on the Serial Monitor the score of Player 1 if its pushbutton 1 was pressed at the correct time
      Serial.println(p1_score);
    }
    
    if(dead_11){
      p1_life-=1;               // deducts 1 life of Player 1
      dead_11 = false;
      Serial.print("P1_life "); // displays on the Serial Monitor the remaining lifes of Player 1 if its pushbutton 1 was pressed at the wrong time
      Serial.println(p1_life);
    }

    if(flag_12){
      p1_score+=1;              // adds 1 point to Player 1 score
      flag_12 = false;
      Serial.print("P12 ");     // displays on the Serial Monitor the score of Player 1 if its pushbutton 2 was pressed at the correct time
      Serial.println(p1_score);
    }

    if(dead_12){
      p1_life-=1;               // deducts 1 life of Player 1
      dead_12 = false;
      Serial.print("P1_life "); // displays on the Serial Monitor the remaining lifes of Player 1 if its pushbutton 2 was pressed at the wrong time
      Serial.println(p1_life);
    }
  }

  if(p2_life!=0){
    if(flag_21){
      p2_score+=1;              // adds 1 point to Player 2 score
      flag_21 = false;
      Serial.print("P21 ");     // displays on the Serial Monitor the score of Player 2 if its pushbutton 1 was pressed at the correct time
      Serial.println(p2_score);
    }

    if(dead_21){
      p2_life-=1;               // deducts 1 life of Player 2
      dead_21 = false;
      Serial.print("P2_life "); // displays on the Serial Monitor the remaining lifes of Player 2 if its pushbutton 1 was pressed at the wrong time
      Serial.println(p2_life);
    }

    if(flag_22){
      p2_score+=1;              // adds 1 point to Player 2 score
      flag_22 = false;
      Serial.print("P22 ");     // displays on the Serial Monitor the score of Player 2 if its pushbutton 2 was pressed at the correct time
      Serial.println(p2_score);
    } 

    if(dead_22){
      p2_life-=1;               // deducts 1 life of Player 2
      dead_22 = false;
      Serial.print("P2_life "); // displays on the Serial Monitor the remaining lifes of Player 2 if its pushbutton 2 was pressed at the wrong time
      Serial.println(p2_life);
    }
  }
}

void t3(void) {
  
  if(p1_life!=0){
    buttonState_11 = read_buttons(B11); // reads the state of the pushbutton 1 of Player 1
    buttonState_12 = read_buttons(B12); // reads the state of the pushbutton 2 of Player 1
  
    if (buttonState_11 == HIGH){        // moment Player 1 pressed its pushbutton 1
    if(game_rows_1[15]==2){
      flag_11=true;
    }else dead_11 = true;
    }
    if (buttonState_12 == HIGH){        // moment Player 1 pressed its pushbutton 2
      if(game_rows_1[15]==1){
        flag_12=true;
      }else dead_12 = true;
    }
  }
}

void t4(void) {
  
  if(p2_life!=0){
    buttonState_21 = read_buttons(B21); // reads the state of the pushbutton 1 of Player 2
    buttonState_22 = read_buttons(B22); // reads the state of the pushbutton 2 of Player 2
  
    if (buttonState_21 == HIGH){        // moment Player 2 pressed its pushbutton 1
      if(game_rows_2[15]==2){
        flag_21=true;
      } else dead_21 = true;
              
    }
    if (buttonState_22 == HIGH){        // moment Player 2 pressed its pushbutton 2
      if(game_rows_2[15]==1){
        flag_22=true;
      } else dead_22 = true;
    }
  }
}

void t5(void) {
  if(global_flag_1==true & global_flag_2==true){ 
    reset = Serial.readString();                 // reads the Input string from Serial Monitor
    reset.trim();                                // cleans up the Input string 
    if(reset=="R"){                              // one of the Players pressed the "R" key in the keyboard
      global_flag_1=false;
      global_flag_2=false;
      global_flag_3=false;
      p1_score=0;                                // resets the score of Player 1
      p2_score=0;                                // resets the score of Player 2
      p1_life=3;                                 // resets the lives of Player 1
      p2_life=3;                                 // resets the lives of Player 2
      lcd1.clear();                              // clears screen of the LCD of Player 1                             
      lcd2.clear();                              // clears screen of the LCD of Player 2
    }
  }
}

void setup() {

  set_up_button(B11);            // initialize the pushbutton 1 pin of Player 1 as an input
  set_up_button(B12);            // initialize the pushbutton 2 pin of Player 1 as an input
  set_up_button(B21);            // initialize the pushbutton 1 pin of Player 2 as an input
  set_up_button(B22);            // initialize the pushbutton 2 pin of Player 2 as an input

  lcd1.clear();                  // clears screen of the LCD of Player 1
  lcd1.begin(16, 2);             // sets up the Player 1 LCD's number of columns and rows
  lcd1.createChar(1,customChar); // creates the game character on the LCD of Player 1
  
  lcd2.clear();                  // clears screen of the LCD of Player 2
  lcd2.begin(16, 2);             // sets up the Player 2 LCD's number of columns and rows
  lcd2.createChar(1,customChar); // creates the game character on the LCD of Player 2

  randomSeed(analogRead(0));     // initializes the pseudo-random number generator

  Serial.begin(9600);

  //INIT SCHEDULER

  Sched_Init();
  Sched_AddT(t1, 0, 500);        // task that refreshes LCDs & executes the game logic
  Sched_AddT(t2, 0, 500);        // task that checks and updates the Score
  Sched_AddT(t3, 0, 50);         // task that reads buttons of Player 1
  Sched_AddT(t4, 0, 50);         // task that reads buttons of Player 2
  Sched_AddT(t5, 0, 5);          // task that resests the game when both players lose
  
  noInterrupts();                // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
 
  //OCR1A  = 31250;              // compare match register 16MHz/256/2Hz
  //OCR1A = 31;                  // compare match register 16MHz/256/2kHz
  OCR1A  = 62.5;                 // compare match register 16MHz/256/1kHz
  TCCR1B |= (1 << WGM12);        // CTC mode
  TCCR1B |= (1 << CS12);         // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);       // enable timer compare interrupt
  interrupts();                  // enable all interrupts
}

void loop() 
{
  // put your main code here, to run repeatedly:
  Sched_Dispatch();
}

//KERNEL CODE
typedef struct{
  int period;         /* period in ticks */
  int delay;          /* ticks until next activation */
  void (*func)(void); /* function pointer */
  int exec;           /* activation counter */
} Sched_Task_t;

#define NT 5
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
      return;                                 //TURNS TO FIXED PRIORITIE
    }
  }
}
