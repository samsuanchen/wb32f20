// ws06.h // having wordset 06 for forth defining word and related
#ifndef WS06_H
#define WS06_H
//////////////////////////////////////////////////////////////////////////
void _doConstant() { V.dsPush(V.getExecWord()->p.con); } // return the constant value
void _doVariable() { V.dsPush((int)V.getExecWord()->p.var); } // return the address of variable
void _doColon() { V.call(V.getExecWord()->p.wpl); } // interpret the wplist of forth colon word
void _doValue() { V.dsPush(*((int *)V.getExecWord()->p.val)); } // return the value 
//void _doIBuf () { V,dsPush((int)V.getExecWord()->p.ibf); } // return the address of n-cell buffer
//void _doCBuf () { V.dsPush((int)V.getExecWord()->p.cbf); } // return the address of n-cell buffer
//////////////////////////////////////////////////////////////////////////
const Word W_doConstant PROGMEM = {LAST, 0, "constant", "(constant)", _doConstant, (int)_doConstant};
//////////////////////////////////////////////////////////////////////////
void _constant() { // : ( <name> -- )
  char* tkn=V.parseToken();
  Word* w = V.create(tkn,_doConstant);
  V.vocAdd(w);
  w->type="constant", w->flag=REVEAL_WORD, w->p.con=V.dsPop();
}
const Word W_constant PROGMEM = {(Word*)&W_doConstant, 0, "primitive", "constant", _constant, 0};
//////////////////////////////////////////////////////////////////////////
const Word W_doVariable PROGMEM = {(Word*)&W_constant, 0, "constant", "(variable)", _doConstant, (int)_doVariable};
//////////////////////////////////////////////////////////////////////////
void _variable() { // : ( <name> -- )
  char* tkn=V.parseToken();
  Word* w = V.create(tkn,_doVariable);
  V.vocAdd(w);
  int* v=(int*)malloc(sizeof(int));
  *v=0;
  w->type="variable", w->flag=REVEAL_WORD, w->p.var=v;
}
const Word W_variable PROGMEM = {(Word*)&W_doVariable, 0, "primitive", "variable", _variable, 0};
//////////////////////////////////////////////////////////////////////////
void _tracing() { V.dsPush((int)V.adrTracing()); } // tracing ( -- &tracing ) get address of tracing depth
const Word W_tracing PROGMEM = {(Word*)&W_variable, 0, "primitive", "tracing", _tracing, 0};
//////////////////////////////////////////////////////////////////////////
void _base() { V.dsPush((int)V.adrBase()); } // base ( -- &B ) get address of B the base of number conversion
const Word W_base PROGMEM = {(Word*)&W_tracing, 0, "primitive", "base", _base, 0};
//////////////////////////////////////////////////////////////////////////
void _store() { int *p=(int*)V.dsPop(); *p=V.dsPop(); } // ! ( n a -- ) store integer n to memory a
const Word W_store PROGMEM = {(Word*)&W_base, 0, "primitive", "!", _store, 0};
//////////////////////////////////////////////////////////////////////////
void _fetch() { V.dsPush(*(int*)V.dsPop()); } // @ ( a -- n ) fetch integer n from memory a
const Word W_fetch PROGMEM = {(Word*)&W_store, 0, "primitive", "@", _fetch, 0};
//////////////////////////////////////////////////////////////////////////
void _cStore() { char*p=(char*)V.dsPop(); *p=V.dsPop(); } // c! ( n a -- ) store char n to memory a
const Word W_cStore PROGMEM = {(Word*)&W_fetch, 0, "primitive", "c!", _cStore, 0};
//////////////////////////////////////////////////////////////////////////
void _cFetch() { V.dsPush(*(char*)V.dsPop()); } // c@ ( a -- n ) fetch char n from memory a
const Word W_cFetch PROGMEM = {(Word*)&W_cStore, 0, "primitive", "c@", _cFetch, 0};
//////////////////////////////////////////////////////////////////////////
void _ret() { // ret ( -- ) ( ip -- ) // 程式結束, 從回返堆疊取出位址 ip, 回到原呼叫的程式
  V.ipSet( (Word**) V.rsPop() );
}
const Word W_ret PROGMEM = {(Word*)&W_cFetch, 0, "primitive", "(;)", _ret, 0};
//////////////////////////////////////////////////////////////////////////
void _lit() {
  Word**ip; int n=(int)*(ip=V.ipGet()+1);
  V.traceData(ip,n);
  V.ipSet(ip), V.dsPush(n);
}
const Word W_lit PROGMEM = {(Word*)&W_ret, 0, "primitive", "(lit)", _lit, 0};
//////////////////////////////////////////////////////////////////////////
void _colon() { // : ( <name> -- )
    Word* w = V.create(V.parseToken(),_doColon);
    V.vocAdd(w);
    w->type="colon", w->flag=HIDE_WORD, V.cpInit(), V.setState(COMPILING);
}
const Word W_colon PROGMEM = {(Word*)&W_lit, 0, "primitive", ":", _colon, 0};
//////////////////////////////////////////////////////////////////////////
void _semicolon() { // ; ( -- )
    V.compile((Word*)&W_ret);
    Word* w=V.vocGet()->last;
    w->flag=REVEAL_WORD;
    w->p.wpl = V.wpClone();
    V.setState(INTERPRETING);
}
const Word W_semicolon PROGMEM = {(Word*)&W_colon, IMMID_WORD, "primitive", ";", _semicolon, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_semicolon
//////////////////////////////////////////////////////////////////////////
#endif WS06_H
