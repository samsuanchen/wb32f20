// ws01.h // having wordset 01 for base setting of number conversion
#ifndef WS01_H
#define WS01_H
//////////////////////////////////////////////////////////////////////////
void _binary () { // 01. bin       ( -- )   set number conversion base B=2
  V.setBase( 2);
}
const Word W_binary PROGMEM={LAST, 0, "primitive", "binary", _binary, 0};
//////////////////////////////////////////////////////////////////////////
void _octal () { // 02. oct       ( -- )   set number conversion base B=8
  V.setBase( 8);
}
const Word W_octal PROGMEM={(Word*)&W_binary, 0, "primitive", "octal", _octal, 0};
//////////////////////////////////////////////////////////////////////////
void _decimal () { // 03. dec       ( -- )   set number conversion base B=10
  V.setBase(10);
}
const Word W_decimal PROGMEM={(Word*)&W_octal, 0, "primitive", "decimal", _decimal, 0};
//////////////////////////////////////////////////////////////////////////
void _hex () { // 04. hex       ( -- )   set number conversion base B=16
  V.setBase(16);
}
const Word W_hex PROGMEM={(Word*)&W_decimal, 0, "primitive", "hex", _hex, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_hex
#endif WS01_H
