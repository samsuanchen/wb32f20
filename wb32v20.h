// wb32v20.h
#ifndef _WB32V20_H_
#define _WB32V20_H_
#include "Arduino.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SHOW_MESSAGE(FORMAT,ID,...) error=ID;tReady=true;Serial.printf(FORMAT,ID,__VA_ARGS__)
#define PRINTF		Serial.printf
#define AVAILABLE	Serial.available
#define READ		  Serial.read
#define WRITE		  Serial.write
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIB_SIZE 	80              // size of terminal input char buffer for collecting input characters
#define TMP_SIZE	256             // size of temporary      char buffer for parsing token or coverting number to digits
#define DS_SIZE   32              // size of data   stack
#define RS_SIZE   32              // size of return stack
#define CS_SIZE   1024            // size of temporary compile space for wplist of a forth colon word
#define SB_SIZE   4096            // size of string buffer for saving all unique strings
#define DS_LIMIT	(DS+DS_SIZE-1)  // data   stack limit
#define RS_LIMIT  (RS+RS_SIZE-1)  // return stack limit
#define CS_LIMIT  (CS+CS_SIZE-1)  // temp  wplist limit
#define SB_LIMIT  (SB+SB_SIZE-1)  // string buffer limit
#define SL_LIMIT  253             // string length limit
#define LAST NULL
#define CONSOLE_WIDTH 80
#define COMPILING     1 // state=1 while in colon definition
#define INTERPRETING  0 // state=0 while out colon definition
#define REVEAL_WORD   0 // w.flag=0 while out colon definition
#define IMMID_WORD    1 // w.flag=1 if the forth word is an immediate word
#define HIDE_WORD     2 // w.flag=2 while in colon definition
/////////////////////////////////////////////////////////////////////////////////////////////////////////
union P {             // using a cell to hold one of the following:
  int            con   ; // the value of forth constant word
  int          * var   ; // the address pointing to int var of forth variable word
  int          * val   ; // the address pointing to int val of forth value word
  int          * ibf   ; // the address pointing to int ibf[] of forth int buffer word
  char         * cbf   ; // the address pointing to char cbf[] of forth char buffer word
  struct Word ** wpl   ; // the address pointing to Word* wpl[] (the word pointer list) of forth colon word
};
typedef void (*FuncP)(); // the forth function pointer type
/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Word { // the forth word type
  struct Word  * prev  ; // the address pointing to previous forth word
  int            flag  ; // the flag info of the forth word
  char         * type  ; // the address pointing to name of the defining word
  char         * name  ; // the address pointing to name of the forth word
  FuncP          code  ; // pointing to the function code to execute
  union P        p     ; // the parameter of the forth word
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Voc {  // the forth vocaburary type
  Word         * last  ; // point to the last defined forth word
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class WB32V20;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class WB32V20 {
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  public:
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
    WB32V20 () {}
    virtual ~WB32V20 () {}
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void showMessage(char *format, uint8_t id, ...){error=id;tReady=true;va_list ap;Serial.printf(format,id,ap);}
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    init (Word* last);      // linking the vocabulary to last and reset data stack and return stack
    boolean isColonWord (Word *w);  // checking if w is a forth word
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    boolean EOL (char c);           // checking if c is end of line
    boolean backSpace (char c);     // checking if c is back space
    boolean whiteSpace (char c);    // checking if c is white space
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void	  dsClear();              // clearing data stack
    void	  dsPush(int n);          // pushing a number onto data stack
    int		  dsPop();                // popping a number from data stack
    int		  dsDepth();              // returing depth of data stack
    boolean dsHasItems (int n);     // checking if data stack having at least n items
    boolean dsHasSpace (int n);     // checking if data stack having enough space for n items
    boolean dsFull();               // checking if data stack full
    void    dsShow();               // showing info of data stack
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    rsClear();              // clearing return stack
    void    rsPush(int n);          // pushing a number onto return stack
    int     rsPop();                // popping a number from return stack
    int     rsDepth();              // returing depth of return stack
    boolean rsHasItems (int n);     // checking if retur stack having at least n items
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    char*   hexPrefix (char *s);    // returning rest string if having hex prefix "0x" or "$"
    char	  toDigit(int x);         // converting integer x into single digit
    char*	  toDigits(uint x, int b);// converting integer x into digits
    int     toNumber(char *token);  // converting token to an integer number
    void    interpret(char* line);  // interpreting given string
    void    traceIP(Word** ip);
    void    traceWord(Word** ip, Word* w);
    void    traceData(Word** ip, int n);
    void    call (Word** wplist);   // inner interpreting wplist of a forth colon word
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    cpInit();               // initializing temporary wplist, setting CP=CS
    void    compile(Word*w);        // compile w into temporary wplist
    Word**  wpClone();              // copying temporary wplist as a new wplist
    void    ipSet(Word**ip);        // setting IP=ip of wplist
    Word**  ipGet();                // get IP
    Word*   wpNext();               // get next wp and increase CP
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    char*   uniqueString (char *s); // making a unique string
    Word*   create(char*n, FuncP f);// creating a forth word of name n with the function code f to execute
    Voc*    vocGet ();              // returning the vocabulary
    Word*   vocSearch (char *name); // searching the forth word of given name in the vocabulary
    void    vocAdd (Word *w);       // adding w to the vocabulary
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    Word**  adrExecWord ();         // get the forth word to execute
    void    setExecWord (Word* w);  // set the forth word to execute
    Word*   getExecWord ();         // get the forth word to execute
    int*    adrBase ();             // get number convertion base B
    void    setBase (int b);        // set b as number convertion base B
    int     getBase ();             // get number convertion base B
    int*    adrState ();            // get state COMPILING/INTERPRETING
    void    setState (int s);       // set s as state COMPILING/INTERPRETING
    int     getState ();            // get state COMPILING/INTERPRETING
    int*    adrError ();            // get error id to see if error
    void    setError (int e);       // set non-zero e as error id
    int     getError ();            // get error id to see if error
    int*    adrTracing ();          // get depth of tracing
    void    setTracing (int t);     // set depth of tracing=t
    int     getTracing ();          // get depth of tracing
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void	  words (char *sub);      // show all word names having specific substring
    void 	  see (Word *w);          // show the forth word w
    void	  dump (int *a, int n);   // dump ( adr n -- ) // dump n cells at adr
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    toValue (Word*w,int n); // store n to w if w is type of "value"
    void    ms (int n);             // wait n milli seconds
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    ipPush ();              // push IP and IP_head to  return stack
    void    ipPop  ();              // pop IP_head and IP from return stack
    void    callBegin (Word*w);     // calling to forth colon word 
    void    callContinue ();        // continue calling the wplist of a forth colon word
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    parseBegin (char* str); // starting to parse
    char*   parseToken ();          // parsing a token
    void    parseContinue ();       // continuing to parse
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    readLineBegin ();       // begining   of read input line to tib
    void    readLineContinue ();    // continuing of read input line to tib
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  private:
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
    char    SB[SB_SIZE];            // string buffer to hold all unique srings
    char*   sbEnd=SB;               // string buffer end
    char    tib[TIB_SIZE];          // terminal input buffer
    char    tmp[TMP_SIZE];          // tmp buffer used in parseToken() and toDigits()
    char*   tmpLimit=tmp+TMP_SIZE-1;//
	  char*   tBegin=tib;             // terminal input buffer begin address
	  char*   tEnd=tib;               // terminal input buffer end   address ( tib is empty, tEnd-tBegin=0 )
	  char*   tLimit=tib+TIB_SIZE-1;  // terminal input buffer limit address ( tEnd <= tLimit )
    boolean tReady=false;           // ready to push char into tib   
    char*   pBegin;                 // string parsing begin address
    char*   pEnd;                   // string parsing end   address
    char*   pLimit=tEnd;            // string parsing limit address
    boolean pReady=false;           // allowing to parse
    char*   cBegin;                 // calling begin IP of the wplist
    boolean cReady=false;           // allowing to call
	  /////////////////////////////////////////////////////////////////////////////////////////////////////
    int     DS[DS_SIZE];            // data stack
    int*    DP;                     // data stack pointer
    int     T;                      // top of data stack
    int     RS[RS_SIZE];            // return stack
    int*    RP;                     // return stack pointer
    Word*   CS[CS_SIZE];            // temporary compile space for a new forth colon word
    Word**  CP;                     // pointing to temporary compile space of a forth colon word
    Word**  IP;                     // pointing to current running wplist of a forth colon word
    Word**  IP_head;                // pointing to current running wplist of a forth colon word
    Voc*    voc;                    // the vocabulary
    Word*   W;                      // pointing to the forth word to execute
    int		  B=10;                   // base for number conversion (at least 2, at most 36)
    int     state=INTERPRETING;     // INTERPRETING or COMPILING
    int     error=0;                // error id
    int     tracing=1;              // tracing depth
    int     checking=1;             // error checking
    int     lineIndex=0;            // index of input line
    int     waitTime=0;             // target time to wait requested by ms(n)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int   led=16,/*LeftButtons*/LB=33,LG=17,LR=32,LY=27,/*RightButtons*/RY=34,RB=35,/*BackButton*/PROG=0;
int   buttons[7]={/*LeftButtons*/LB,LG,LR,LY,/*RightButtons*/RY,RB,/*BackButton*/PROG};
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif _WB32V20_H_
