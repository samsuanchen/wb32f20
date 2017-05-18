// ws03.h // having wordset 03 for tools
#ifndef WS03_H
#define WS03_H
//////////////////////////////////////////////////////////////////////////
void _words () { // 11. words          ( -- ) show all forth word names
                 //     words <string> ( -- ) show all forth word names including given string
  V.words(V.parseToken()); 
}
const Word W_words PROGMEM={LAST, 0,"primitive", "words", _words, 0};
//////////////////////////////////////////////////////////////////////////
void _see   () { // 12. see <name>     ( -- ) see the word of given name
  V.see(V.vocSearch(V.parseToken())) ;
}
const Word W_see   PROGMEM={(Word*)&W_words, 0, "primitive", "see", _see, 0};
//////////////////////////////////////////////////////////////////////////
void _dump  () { // 13. dump           ( a n -- ) show n cells at address a
  X=V.dsPop(); V.dump((int*)V.dsPop(),X);
}
const Word W_dump  PROGMEM={(Word*)&W_see, 0, "primitive", "dump", _dump, 0};
//////////////////////////////////////////////////////////////////////////
void _trace  () { // 13. trace <name>  ( ... -- ... ) show the calling sequence of give forth colon word
  int t = V.getTracing();
  V.setTracing(1); 
  char* name = V.parseToken();
  if ( ! *name ) {
    V.setError(002);
    PRINTF("\nerror 002 ??? no name after trace ??? ");
    return;
  }
  PRINTF("\ntracing %s ",name);
  Word* w = V.vocSearch(name);
  if ( ! w ) {
    V.setError(003);
    PRINTF("\nerror 003 ??? %s undefined ??? ", name);
    return;
  }
  w->code();
  V.setTracing(t);
}
const Word W_trace  PROGMEM={(Word*)&W_dump, 0, "primitive", "trace", _trace, 0};
//////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W_trace
#endif WS03_H
