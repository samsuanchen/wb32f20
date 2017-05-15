// ws04.h // having wordset 04 for digital output
#ifndef WS04_H
#define WS04_H
//////////////////////////////////////////////////////////////////////////
void _led(){ // 11. words          ( -- ) show all forth word names
  V.dsPush(16);
}
const Word W_led PROGMEM={LAST, 0, "primitive", "led", _led, 0};
//////////////////////////////////////////////////////////////////////////
void _out(){ // 11. words          ( -- ) show all forth word names
  pinMode(V.dsPop(),OUTPUT);
}
const Word W_out PROGMEM={(Word*)&W_led, 0, "primitive", "output", _out, 0};
//////////////////////////////////////////////////////////////////////////
void _hi (){ // 11. words          ( -- ) show all forth word names
  digitalWrite(V.dsPop(),HIGH);
}
const Word W_hi PROGMEM={(Word*)&W_out, 0, "primitive", "high", _hi, 0};
//////////////////////////////////////////////////////////////////////////
void _lo (){ // 11. words          ( -- ) show all forth word names
  digitalWrite(V.dsPop(),LOW);
}
const Word W_lo PROGMEM={(Word*)&W_hi, 0, "primitive", "low", _lo, 0};
//////////////////////////////////////////////////////////////////////////
void _pinSet(){ // 11. words          ( -- ) show all forth word names
  digitalWrite(V.dsPop(),V.dsPop());
}
const Word W_pinSet PROGMEM={(Word*)&W_lo, 0, "primitive", "pin!", _pinSet, 0};
//////////////////////////////////////////////////////////////////////////
void _ms(){ // 11. words          ( -- ) show all forth word names
  delay(V.dsPop());
}
const Word W_ms PROGMEM={(Word*)&W_pinSet, 0, "primitive", "ms", _ms, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_ms
#endif WS04_H
