// ws05.h // having wordset 05 for digital input
#ifndef WS05_H
#define WS05_H
//////////////////////////////////////////////////////////////////////////
void _inp(){ // 20. inp ( pin -- ) set gpio pin mode direction as INPUT
  pinMode(V.dsPop(),INPUT);
}
const Word W_inp PROGMEM = {LAST, 0, "primitive", "input", _inp, 0};
//////////////////////////////////////////////////////////////////////////
void _pinGet(){ // 21. p@ ( pin -- v ) read v from digital INPUT pin (v=0 for LOW or v=1 for HIGH)
  V.dsPush(digitalRead(V.dsPop()));
} 
const Word W_pinGet PROGMEM = {(Word*)&W_inp, 0, "primitive", "pin@", _pinGet, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_pinGet
#endif WS05_H
