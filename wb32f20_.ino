// wb32f20.ino // wifi boy esp32 forth proj 020 // simple interactive system having 26 forth words
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "wb32v20.h"
WB32V20 V = WB32V20(); // having V as an instence of wb32v20 stack machine
void setup() {
  Serial.begin(115200); // setting serial communification baud rate
  init(); // initializing V (the forth Virtual machine) to have 26 forth words
  Serial.printf("==================================================\n");
  Serial.printf("Simple Wifi Boy ESP32 System having 26 Forth Words\n");
  Serial.printf("20170516 derek@wifiboy.org & samsuanchen@gmail.com\n");
  Serial.printf("==================================================\n");
  V.interpret("0x100 100 2 $20 -1 words"); // turning led on, preloading 5 numbers, and showing words
  V.readLineBegin();
}
int led=16,/*LeftButtons*/LB=33,LG=17,LR=32,LY=27,/*RightButtons*/RY=34,RB=35,/*BackButton*/PROG=0;
void loop() {
  if(!digitalRead(RB))digitalWrite(led,HIGH );               // turning off the led by button RB the right blue  
  if(!digitalRead(RY))digitalWrite(led,LOW  );               // turning on  the led by button RY the right yellow
  if(!digitalRead(LG))digitalWrite(led,1-digitalRead(led));  // toggling    the led by button LG the left  green
//V.interpret(line);                  // interpreting input line
  next();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define tibSize 1024
#define tibLimit (tib+tibSize-1)
char tib[tibSize], *tibBegin = tib, *tibEnd = tib;
char *parseBegin, *parseRemain, *token;
Word *W, **IP;
unsigned long targetTime=0;

void delayBegin    ( int n ) { targetTime=n+millis(); }
void delayContinue (       ) {
  
}


void interpretBegin   (char *s) { parseBegin = parseRemain = s; }
void interpretContinue(       ) {
  if ( IP )                   return;  // calling not finished
  if ( ! *parseRemain ) {              // end of interpreting
    if ( parseBegin == tib ){             // interpreting tib finished
      V.readLineBegin();                       // begin to read a line
    }
    return;
  }
  token = V.parseToken();
  W = V.vocSearch(token);
  if ( W ) {
    if ( V.getState() == INTERPRETING || W->flag == IMMID_WORD ) {
      if ( V.isColonWord(W) ) {
        V.call(W->p.wpl);             return;  // call the forth colon word
      }
      W->code(); // execute the forth non-colon word
    } else {             // COMPILING && W->flag != IMMID_WORD
      V.compile(W);   return; // compile W
    }
  }
  int n = V.toNumber(token); // token a not forth word
  if( V.getError() )  return;
  if( V.getState() == INTERPRETING ) {
    V.dsPush(n);      return;
  }
  Word *W_doLit;
  if ( ! W_doLit ) W_doLit = V.vocSearch("(lit)");
  V.compile(W_doLit);
  V.compile((Word*)n);
}
void callBegin    (Word *w) { V.rsPush((int)IP); IP=w->p.wpl; }
void callContinue (       ) {
  if( ! IP ) return; // end of calling
  W=*IP;
  if ( V.isColonWord(W) ) {
    V.call(W->p.wpl);             return;  // call the forth colon word
  }
  W->code(); // execute the forth non-colon word
}
void next() {
  V.readLineContinue();
//if ( millis() < targetTime ) return; // waiting until targetTime
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
  PRINTF("The last forth word defined at 0x%x\n",LAST); // showing address of the last forth word
  V.init(LAST); // initializing V (the forth Virtual machine) to have 26 forth words
}
