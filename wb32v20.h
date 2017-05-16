// wb32v10.h
#ifndef _WB32V10_H_
#define _WB32V10_H_
#include "Arduino.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PRINTF		Serial.printf
#define AVAILABLE	Serial.available
#define READ		  Serial.read
#define WRITE		  Serial.write
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIB_SIZE 	1024
#define TMP_SIZE	256
#define DS_DEPTH	32
#define RS_DEPTH  32
#define CS_LEN    1024
#define SB_LEN    4096
#define DS_LIMIT	(DS+DS_DEPTH-1) // data   stack limit
#define RS_LIMIT  (RS+RS_DEPTH-1) // return stack limit
#define CS_LIMIT  (CS+CS_LEN  -1) // temp  wplist limit
#define SB_LIMIT  (SB+SB_LEN  -1) // string buffer limit
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
class WB32V10;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class WB32V10 {
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  public:
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
    WB32V10 () {}
    virtual ~WB32V10 () {}
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    init (Word* last);      // linking the vocabulary to last and reset data stack and return stack
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    boolean EOL (char c);           // checking if c is end of line
    boolean backSpace (char c);     // checking if c is back space
    boolean whiteSpace (char c);    // checking if c is white space
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    boolean tibEmpty ();            // checking if buffer is empty
    boolean tibFull ();             // checking if buffer is full
    void    tibOpen ();             // starting to collect input characters
    void    tibClose ();            // adding '\0' as end of input string
    void    tibPop ();              // poping last input character
    void    tibPush (char c);       // collecting input character
    /////////////////////////////////////////////////////////////////////////////////////////////////////
  	void    waitInput();            // waiting for input
  	char*   readLine ();            // reading an input line
    /////////////////////////////////////////////////////////////////////////////////////////////////////
  	void    parseBegin (char* str); // starting to parse
  	char    parseAvailable ();      // returning non '\0' means available to parse
  	char*   parseToken ();          // parsing a token
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
    void    eval(char *token);      // evaluating given token
    void    interpret(char* line);  // interpreting given string
    void    traceIP(Word** ip);
    void    traceWord(Word** ip, Word* w);
    void    traceData(Word** ip, int n);
    void    call (Word** wplist);   // inner interpreting wplist of a forth colon word
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void    cpInit();               // initializing temporary wplist, setting CP=CS
    void    wpPush(Word*w);         // pushing w into temporary wplist
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
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  private:
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
    char    SB[SB_LEN];             // string buffer to hold all unique srings
    char*   sbEnd=SB;               // string buffer end
    char    tib[TIB_SIZE];          // terminal input buffer
    char    tmp[TMP_SIZE];          // tmp buffer used in parseToken() and toDigits()
	  char*   tibBegin;               // terminal input buffer begin
	  char*   tibEnd;                 // terminal input buffer end
	  char*   tibLimit;               // terminal input buffer limit
    char*   parseRemain;            // terminal input buffer remain for parsing
	  /////////////////////////////////////////////////////////////////////////////////////////////////////
    int     DS[DS_DEPTH];           // data stack
    int*    DP;                     // data stack pointer
    int     T;                      // top of data stack
    int     RS[RS_DEPTH];           // return stack
    int*    RP;                     // return stack pointer
    Word*   CS[CS_LEN];             // temporary compile space for a new forth colon word
    Word**  CP;                     // pointing to temporary compile space of a forth colon word
    Word**  IP;                     // pointing to current running wplist of a forth colon word
    Voc*    voc;                    // the vocabulary
    Word*   W;                      // pointing to the forth word to execute
    int		  B=10;                   // base for number conversion (at least 2, at most 36)
    int     state=INTERPRETING;     // INTERPRETING or COMPILING
    int     error;                  // error id
    int     tracing=0;              // tracing depth
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
};
#endif _WB32V10_H
