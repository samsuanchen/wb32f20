// ws06.h // having wordset 06 for forth defining word and related
#ifndef WS06_H
#define WS06_H
//////////////////////////////////////////////////////////////////////////
void _doConstant() { V.dsPush(V.getExecWord()->p.con); } // return the constant value
void _doVariable() { V.dsPush((int)V.getExecWord()->p.var); } // return the address of variable
void _doColon() { V.callBegin(V.getExecWord()); } // interpret the wplist of forth colon word
void _doValue() { V.dsPush(*((int *)V.getExecWord()->p.val)); } // return the value 
//void _doIBuf () { V,dsPush((int)V.getExecWord()->p.ibf); } // return the address of n-cell buffer
//void _doCBuf () { V.dsPush((int)V.getExecWord()->p.cbf); } // return the address of n-cell buffer
//////////////////////////////////////////////////////////////////////////
const Word W_doConstant PROGMEM = {LAST, 0, "constant", "(constant)", _doConstant, (int)_doConstant};
//////////////////////////////////////////////////////////////////////////
const Word W_doVariable PROGMEM = {(Word*)&W_doConstant, 0, "constant", "(variable)", _doConstant, (int)_doVariable};
//////////////////////////////////////////////////////////////////////////
const Word W_doColon PROGMEM = {(Word*)&W_doVariable, 0, "constant", "(:)", _doConstant, (int)_doColon};
//////////////////////////////////////////////////////////////////////////
const Word W_doValue PROGMEM = {(Word*)&W_doColon, 0, "constant", "(value)", _doConstant, (int)_doValue};
//////////////////////////////////////////////////////////////////////////
int _tmpValue=1234;
const Word W_tmpValue PROGMEM = {(Word*)&W_doValue, 0, "value", "tmpValue", _doValue, (int)&_tmpValue};
//////////////////////////////////////////////////////////////////////////
void _pto() { // (to) ( n -- ) compileonly
  Word **ip=V.ipGet();
  V.toValue(*(++ip),V.dsPop());
  V.ipSet(ip);
}
const Word W_pto PROGMEM = {(Word*)&W_tmpValue, 0, "primitive", "(to)", _pto, 0};
//////////////////////////////////////////////////////////////////////////
void _to() { // to <name> ( n -- )
  if(V.getTracing())PRINTF("to ");
  char *name=V.parseToken();
  if(V.getTracing())PRINTF("\nexecuting \"to %s\" ",name);
  if (*name) {
    Word *w=V.vocSearch(name); // find the forth word w of given name
    if (w) {
      int state = V.getState();
      if(V.getTracing())PRINTF("of type %s at state %d ",w->type, state);
      if ( state == INTERPRETING ) {
        int n=V.dsPop();
        V.toValue(w,n);  // store an integer to w if it is a forth value word
      } else { // COMPILING
        if( strcmp(w->type,"value") ) {
          V.showMessage("\n??? error %03d ??? not forth value word to store value\n",103);
        } else {
          V.compile((Word*)&W_pto), V.compile(w);
        }
      }
    } else {
      V.showMessage("\n??? error %03d ??? no name given to store value\n",103);
    }
  } else {
  }
}
const Word W_to PROGMEM = {(Word*)&W_pto, 0, "primitive", "to", _to, 0};
//////////////////////////////////////////////////////////////////////////
void _constant() { // : ( <name> -- )
  char* tkn=V.parseToken();
  Word* w = V.create(tkn,_doConstant);
  V.vocAdd(w);
  w->type="constant", w->flag=REVEAL_WORD, w->p.con=V.dsPop();
}
const Word W_constant PROGMEM = {(Word*)&W_to, 0, "primitive", "constant", _constant, 0};
//////////////////////////////////////////////////////////////////////////
void _variable() { // : ( <name> -- )
  char* tkn=V.parseToken();
  Word* w = V.create(tkn,_doVariable);
  V.vocAdd(w);
  int* v=(int*)malloc(sizeof(int));
  *v=0;
  w->type="variable", w->flag=REVEAL_WORD, w->p.var=v;
}
const Word W_variable PROGMEM = {(Word*)&W_constant, 0, "primitive", "variable", _variable, 0};
//////////////////////////////////////////////////////////////////////////
void _tracing() { V.dsPush((int)V.adrTracing()); } // tracing ( -- &tracing ) get address of tracing depth
const Word W_tracing PROGMEM = {(Word*)&W_variable, 0, "primitive", "tracing", _tracing, 0};
//////////////////////////////////////////////////////////////////////////
void _cStore() { char*p=(char*)V.dsPop(); *p=V.dsPop(); } // c! ( n a -- ) store char n to memory a
const Word W_cStore PROGMEM = {(Word*)&W_tracing, 0, "primitive", "c!", _cStore, 0};
//////////////////////////////////////////////////////////////////////////
void _cFetch() { V.dsPush(*(char*)V.dsPop()); } // c@ ( a -- n ) fetch char n from memory a
const Word W_cFetch PROGMEM = {(Word*)&W_cStore, 0, "primitive", "c@", _cFetch, 0};
//////////////////////////////////////////////////////////////////////////
void _ret() { V.ipPop(); } // (;) ( -- ) return from wplist of forth colon word
const Word W_ret PROGMEM = {(Word*)&W_cFetch, 0, "primitive", "(;)", _ret, 0};
//////////////////////////////////////////////////////////////////////////
void _lit() {
  Word **ip; int n=(int)*(ip=V.ipGet()+1);
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
