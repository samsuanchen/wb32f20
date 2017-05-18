// ws01.h // having wordset 01 for base setting of number conversion
#ifndef WS01_H
#define WS01_H
//////////////////////////////////////////////////////////////////////////
void _binary () {   // 01. binary   ( -- )         store  2 to system variable B (number conversion base)
  V.setBase( 2);
}
const Word W_binary PROGMEM={LAST, 0, "primitive", "binary", _binary, 0};
//////////////////////////////////////////////////////////////////////////
void _octal () {    // 02. octal    ( -- )         store  8 to system variable B (number conversion base)
  V.setBase( 8);
}
const Word W_octal PROGMEM={(Word*)&W_binary, 0, "primitive", "octal", _octal, 0};
//////////////////////////////////////////////////////////////////////////
void _decimal () {  // 03. decimal  ( -- )         store 10 to system variable B (number conversion base)
  V.setBase(10);
}
const Word W_decimal PROGMEM={(Word*)&W_octal, 0, "primitive", "decimal", _decimal, 0};
//////////////////////////////////////////////////////////////////////////
void _hex () {      // 04. hex      ( -- )         store 16 to system variable B (number conversion base)
  V.setBase(16);
}
const Word W_hex PROGMEM={(Word*)&W_decimal, 0, "primitive", "hex", _hex, 0};
//////////////////////////////////////////////////////////////////////////
void _base () {      // 05. base    ( -- a )       get address of B the base of number conversion
  V.dsPush((int)V.adrBase());
}
const Word W_base PROGMEM = {(Word*)&W_hex, 0, "primitive", "base", _base, 0};
//////////////////////////////////////////////////////////////////////////
void _store () {     // 06. !       ( n a -- )     store n to address a (see hardware config for available a)
  int *p=(int*)V.dsPop(); *p=V.dsPop();
}
const Word W_store PROGMEM = {(Word*)&W_base, 0, "primitive", "!", _store, 0};
//////////////////////////////////////////////////////////////////////////
void _fetch () {     // 07. @       ( a -- n )     get n from address a (see hardware config for available a)
  V.dsPush(*(int*)V.dsPop());
}
const Word W_fetch PROGMEM = {(Word*)&W_store, 0, "primitive", "@", _fetch, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_store
#endif WS01_H
