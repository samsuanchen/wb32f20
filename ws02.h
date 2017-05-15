// ws02.h // having wordset 02 for arithmetic operation
#ifndef WS02_H
#define WS02_H
//////////////////////////////////////////////////////////////////////////
void _add () { // 07. + ( a b -- a+b )  a add b
  V.dsPush( V.dsPop() + V.dsPop() );
}
const Word W_add PROGMEM={LAST, 0, "primitive", "+", _add, 0};
//////////////////////////////////////////////////////////////////////////
void _mul () { // 08. * ( a b -- a*b )  a multiply b
  V.dsPush( V.dsPop() * V.dsPop() );
}
const Word W_mul PROGMEM={(Word*)&W_add, 0, "primitive", "*", _mul, 0};
//////////////////////////////////////////////////////////////////////////
int   X;
//////////////////////////////////////////////////////////////////////////
void _sub () { // 09. - ( a b -- a-b )  a subtract b
  X=V.dsPop(); V.dsPush( V.dsPop() - X );
}
const Word W_sub PROGMEM={(Word*)&W_mul, 0, "primitive", "-", _sub, 0};
//////////////////////////////////////////////////////////////////////////
void _div () { // 10. / ( a b -- a/b )  a divide b
  X=V.dsPop(); V.dsPush( V.dsPop() / X );
}
const Word W_div PROGMEM={(Word*)&W_sub, 0, "primitive", "/", _div, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_div
#endif WS02_H
