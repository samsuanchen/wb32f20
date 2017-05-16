// wb32f20.ino // wifi boy esp32 forth proj 020 // simple interactive system having 21 forth words
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "wb32v20.h"
WB32V20 V = WB32V20(); // having V as an instence of wb32v20 stack machine
int buttons[7]={33, 17, 32, 27, 34, 35, 0}; //  4 left buttons LB, LG, LR, LY   2 right buttons RB, RY  back button PROG 
void setup() {
  Serial.begin(115200); // setting serial communification baud rate
  init(); // initializing V (the forth Virtual machine) to have 26 forth words
  for(int i=0; i<7; i++) pinMode(buttons[i],INPUT);  // setting pin mode direction of all the buttons as INPUT
  Serial.printf("==================================================\n");
  Serial.printf("Simple Wifi Boy ESP32 System having 26 Forth Words\n");
  Serial.printf("20170516 derek@wifiboy.org & samsuanchen@gmail.com\n");
  Serial.printf("==================================================\n");
  V.interpret("led output led low 0x100 100 2 $20 -1 words"); // turning led on, preloading 5 numbers, and showing words
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  static int i=0; // setting index of input line
  pinMode(16,INPUT); int led=digitalRead(16); pinMode(16,OUTPUT); // switch mode to read led
  int bLB=digitalRead(33),bLG=digitalRead(17),bLR=digitalRead(32), bLY=digitalRead(27); // read left  buttons
  int bRB=digitalRead(34),bRY=digitalRead(35),bPR=digitalRead( 0); // read right buttons
  Serial.printf("led %d buttons LB %d LG %d LR %d LY %d RB %d RY %d PR %d\n",led,bLB,bLG,bLR,bLY,bRB,bRY,bPR);
  if(!bRB)digitalWrite(16,HIGH );   // turning off the led
  if(!bRY)digitalWrite(16,LOW  );   // turning on  the led
  if(!bLG)digitalWrite(16,1-led);   // toggling    the led
  V.dsShow();                // showing depth, numbers, and number coversion base of data stack 
  Serial.printf("--------------------------------------------------\n");
  Serial.printf("line %02d >> ", i++); // asking for input line i
  V.waitInput();                    // waiting until input available
  char *line = V.readLine();        // reading input line (until caridge return)
  Serial.printf("\n");
  V.interpret(line);                // interpreting input line
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ws01.h" // == including wordset 01 == base setting of number conversion
// 01. binary           ( -- )         number conversion base B=2
// 02. octal            ( -- )         number conversion base B=8
// 03. decimal          ( -- )         number conversion base B=10
// 04. hex              ( -- )         number conversion base B=16
// 05. base !           ( b -- )       number conversion base B=b
// 06. base @           ( -- B )       get number conversion base B
#include "ws02.h" // == including wordset 02 == arithmetic operation
// 07. +                ( a b -- a+b ) a add b
// 08. *                ( a b -- a*b ) a multiply b
// 09. -                ( a b -- a-b ) a subtract b
// 10. /                ( a b -- a/b ) a divide b
#include "ws03.h" // == including wordset 03 == tools
// 11. words            ( -- )         show all forth word names
//     words <string>   ( -- )         show all forth word names including given string
// 12. see <name>       ( -- )         see the word of given name
// 13. dump             ( a n -- )     show n cells at address a
#include "ws04.h" // == including wordset 04 == digital output
// 14. led              ( -- 16  )     give decimal 16 (gpio pin number of the blue led)
// 15. high             ( pin -- )     pull up digital OUTPUT pin to electric level HIGH
// 16. low              ( pin -- )     pull down digital OUTPUT pin to electric level LOW
// 17. ms               ( n -- )       wait for n milliseconds
// 18. output           ( pin -- )     set gpio pin mode direction as OUTPUT
// 19. pin!             ( v pin -- )   write v to digital OUTPUT pin (v=0 for LOW or v=1 for HIGH)
#include "ws05.h" // == including wordset 05 == digital input
// 20. input            ( pin -- )     set gpio pin mode direction as INPUT
// 21. pin@             ( pin -- v )   read v from digital INPUT pin (v=0 for LOW or v=1 for HIGH)
#include "ws06.h" // == including wordset 06 == for testing of forth colon word
// 22. (constant)       ( -- a )       get address of (constant) doing the function of a forth constant word
// 23. constant <name>  ( n -- )       define n as forth constat word of given name
// 24. (variable)       ( -- a )       get address of (variable) doing the function of a forth variable word
// 25. variable <name>  ( -- )         define a forth variable word of given name
// 26. tracing          ( -- depth )   get forth colon word tracing depth (0 means no tracing)
// 27. base             ( -- a )       get address of B the base of number conversion
// 23. (;)              ( -- )         return from wplist of forth colon word
// 24. (lit)            ( -- n )       push n gotten from next cell to data stack
// 25. : <name>         ( -- )         begin to define forth colon word of given name
// 26. ;                ( -- )         end of forth colon word definition
////////////////////////////////////////////////////////////////////////////////////////////////////////
void init(){
  PRINTF("LAST=0x%x\n",LAST); // showing address of the last forth word
  V.init(LAST); // initializing V (the forth Virtual machine) to have 26 forth words
}

