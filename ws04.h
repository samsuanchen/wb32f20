// ws04.h // having wordset 04 for digital output
#ifndef WS04_H
#define WS04_H
//////////////////////////////////////////////////////////////////////////
void _led () {    // 14. led    ( -- 16  )     give decimal 16 (gpio pin number of the blue led)
  V.dsPush(16);
}
const Word W_led PROGMEM={LAST, 0, "primitive", "led", _led, 0};
//////////////////////////////////////////////////////////////////////////
void _high () {   // 15. high   ( pin -- )     pull digital OUTPUT pin to electric level HIGH
  digitalWrite(V.dsPop(),HIGH);
}
const Word W_high PROGMEM={(Word*)&W_led, 0, "primitive", "high", _high, 0};
//////////////////////////////////////////////////////////////////////////
void _low () {    // 16. low    ( pin -- )     pull digital OUTPUT pin to electric level LOW
  digitalWrite(V.dsPop(),LOW);
}
const Word W_low PROGMEM={(Word*)&W_high, 0, "primitive", "low", _low, 0};
//////////////////////////////////////////////////////////////////////////
void _toggle () { // 17. toggle ( pin -- ) pull digital OUTPUT pin to electric level TOGGLE
  int pin=V.dsPop();
  digitalWrite(pin,1-digitalRead(pin));
}
const Word W_toggle PROGMEM={(Word*)&W_low, 0, "primitive", "toggle", _toggle, 0};
//////////////////////////////////////////////////////////////////////////
void _ms () {     // 18. ms     ( n -- )       wait for n milliseconds
  V.ms(V.dsPop());
}
const Word W_ms PROGMEM={(Word*)&W_toggle, 0, "primitive", "ms", _ms, 0};
//////////////////////////////////////////////////////////////////////////
void _output () { // 19. output ( pin -- )     set gpio pin mode direction as OUTPUT
  pinMode(V.dsPop(),OUTPUT);
}
const Word W_output PROGMEM={(Word*)&W_ms, 0, "primitive", "output", _output, 0};
//////////////////////////////////////////////////////////////////////////
void _pinSet () { // 20. pin!   ( v pin -- )   write v to digital OUTPUT pin (v=0 for LOW or v=1 for HIGH)
  digitalWrite(V.dsPop(),V.dsPop());
}
const Word W_pinSet PROGMEM={(Word*)&W_output, 0, "primitive", "pin!", _pinSet, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_pinSet
#endif WS04_H
