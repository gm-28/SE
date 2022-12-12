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

int buttonState_11; // the current reading from the input pin
int buttonState_12;
int buttonState_21;            
int buttonState_22;            

int p1_score = 0;
int p2_score = 0;

bool flag_11 = false;
bool flag_12 = false;
bool flag_21 = false;
bool flag_22 = false;

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
  if(flag_11){
    p1_score+=1;
    flag_11 = false;
    Serial.print("P11 ");
    Serial.println(p1_score);
  }
  else if(flag_12){
    p1_score+=1;
    flag_12 = false;
    Serial.print("P12 ");
    Serial.println(p1_score);
  }

  if(flag_21){
    p2_score+=1;
    flag_21 = false;
    Serial.print("P21 ");
    Serial.println(p2_score);
  }
  else if(flag_22){
    p2_score+=1;
    flag_22 = false;
    Serial.print("P22 ");
    Serial.println(p2_score);
  }
}

void t3(void) {
  buttonState_11 = read_buttons(B11); // read the state of the pushbutton value
  buttonState_12 = read_buttons(B12);
  buttonState_21 = read_buttons(B21);
  buttonState_22 = read_buttons(B22);

  if (buttonState_11 == HIGH){
    if(game_rows_1[15]==2){
      flag_11=true;
    }
  }
  else if (buttonState_12 == HIGH){
    if(game_rows_1[15]==1){
      flag_12=true;
    }
  }

  //Codigo correto? verificar; 
  //Se descomentar esta porção causa um erro, P12 sempre a ser impresso; Ao adicionar um print para analisar LCS bugam; task demasiado grande?!
  //Testar so estas condiçoes com as de cima comentadas causa reset do score semelhante ao erro q tavamos a tentar antes;
  //Butao 21 n funciona
  /*if (buttonState_21 == HIGH){
    if(game_rows_2[15]==2){
      flag_21=true;
    }
  }
  else if (buttonState_22 == HIGH){
    if(game_rows_2[15]==1){
      flag_22=true;
    }
  }*/
}

void setup() {

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

  randomSeed(analogRead(0)); // initializes the pseudo-random number generator

  Serial.begin(9600);

  //INIT SCHEDULER

  Sched_Init();
  Sched_AddT(t1, 0, 500); //Task that refreshes LCDs & Executes Game Logic
  Sched_AddT(t2, 0, 500); //Task that checks and updates Score
  Sched_AddT(t3, 0, 50);  //Task that reads buttons
  //Sched_AddT(t4, 0, 500);
  
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
 
  //OCR1A  = 31250; // compare match register 16MHz/256/2Hz
  //OCR1A = 31;    // compare match register 16MHz/256/2kHz
  OCR1A  = 62.5; // compare match register 16MHz/256/1kHz
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts
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