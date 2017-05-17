#include "wb32v20.h"
/////// --------------------------------- /////////////////////////////////////////////////////////////////
boolean WB32V20::EOL (char c)           { return c=='\n'||c=='\r'; } // check if c is end of line
boolean WB32V20::backSpace (char c)     { return c=='\b'; } // check if c is back space
boolean WB32V20::whiteSpace (char c)    { return c==' '||c=='\t'||c=='\n'||c=='\r'; } // check if c is white space
/////// --------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::tibOpen ()             { tBegin=tEnd=tib; tReady=true; } // init tib waiting for input
void    WB32V20::tibClose ()            { *tEnd='\0'; } // add '\0' at end of input
void    WB32V20::tibPop ()              { --tEnd; } // pop last input character
void    WB32V20::tibPush (char c)       { *(tEnd++)=c; } // collect input character
boolean WB32V20::tibEmpty ()            { return tEnd==tBegin; } // check if buffer is empty
boolean WB32V20::tibFull ()             { return tEnd==tLimit; } // check if buffer is full
/////// --------------------------------- /////////////////////////////////////////////////////////////////
void	  WB32V20::waitInput ()           {  while ( !AVAILABLE() ); } // read input characters until end of line
char*   WB32V20::readLine ()            { // read input characters until end of line
  tibOpen();
  while ( AVAILABLE() ) {
    char c=READ(); // get input char
    if ( backSpace(c) ) { // if backspace
      if ( !tibEmpty() ) tibPop(), PRINTF("\b \b");    // erase last input char
    } else if ( EOL(c) || tibFull() ) {
      tibPush('\0');
      return tib;
    } else {
      tibPush(c); // collect character c
      WRITE(c);
      waitInput(); // wait until input available
    }
  }
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::parseBegin (char *str) { pEnd=pBegin=str; pLimit=tEnd; pReady=1; tReady=0; }
char    WB32V20::parseAvailable ()      { return *pEnd; } // non '\0' means available to parse
char*	  WB32V20::parseToken ()          { // use white spaces as delimiters to parse a token
  char c = *pEnd;
  while ( c && whiteSpace(c) ) c = *++pEnd; // ignore leading white spaces
  char *start = pEnd; // set start at non white space
  if ( c ) {
    while ( c && !whiteSpace(c) ) c = *++pEnd; // colect  non white spaces
    char *limit = pEnd; // set limit at white space
    if ( c ) { // if not end of string
      int n=limit-start;
      if ( n>TMP_SIZE-1 ) { //
      	PRINTF("\n error WB32V20::parseToken length %d > %d\n",n,TMP_SIZE-1);
      	return start;
      }
      strncpy(tmp,start,n);
      tmp[n] = '\0'; // add null as the end of string
      start = tmp;
    }
  }
  return start;
}
void WB32V20::traceIP(Word** ip){
  int depth=rsDepth();
  PRINTF("tracing %08x %08x ",ip,W);
  for(int i=1; i<depth; i++) PRINTF("| ");
}
void WB32V20::traceWord(Word** ip,Word* w){
  if(tracing){
    traceIP(ip);
    PRINTF("%s\n",W->name);
  }
}
void WB32V20::traceData(Word**ip,int n){
  if(tracing){
    traceIP(ip);
    PRINTF("%d\n",n);
  }
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::call (Word**wplist)    { // inner interpreting the wplist of a forth colon word
  rsPush((int)IP);
  IP=wplist;
  while(IP){
    W=*IP;
    traceWord(IP,W);
    W->code();
    if(IP)IP++;
  }
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void	  WB32V20::interpret (char *line) { // interpreting given string
  pBegin = pEnd = line;  // no more input until end of interpreting
  pLimit = line+strlen(line); 
  parseBegin(line);
  while ( *pEnd ) {
    char *token = parseToken();
    char *remain, *p=token, c;
    W=vocSearch(token);
    Word* wLit;
    if(!wLit) wLit=vocSearch("(lit)");
    if(W){
      if(state==INTERPRETING || W->flag==IMMID_WORD){
        IP=0;
        W->code();
      } else {
        compile(W); 
      }
    } else {
      int n=toNumber(token);
      if(!getError()){
        if(state==INTERPRETING){
          dsPush(n);
        } else {
          compile(wLit);
          compile((Word*)n);
        }
      }
    }
  }
  readLineBegin(); 
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
int	WB32V20::toNumber(char *token){
  char *remain, *p, c;
  int b, n;
  if(remain=hexPrefix(token)){
    p=remain, b=16;
  } else {
    p=token, b=getBase();
  }
  n=strtol(p, &remain, b); // conver string at p to integer n on base b (*remain is an illigal digit)
  if(c=*remain){
    error=100;
    PRINTF("\nerr %02d %s ? illigal '%c' at %d as base%d digit",error,token,c,remain-token,b);
  }
  return n;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void	WB32V20::dsShow(){ // show data stack info
  PRINTF("< dsDepth %d [ ",dsDepth()); // show depth
  if(dsDepth()>0){
    if(dsDepth()>5)PRINTF(".. "); // showing at most top 5 items
    for ( int *i=max(DP-4,DS); i<=DP; i++ ) PRINTF("%s ",toDigits(*i,B)); // show data
  }
  PRINTF("] base%d >\n",B); // show base 
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
boolean WB32V20::dsFull     ()      { return DP   >= DS+DS_SIZE; } // check if data stack full
boolean WB32V20::dsHasSpace (int n) { return DP+n <= DS+DS_SIZE; } // check if data stack has space for n items
boolean WB32V20::dsHasItems (int n) { return DP >= DS+n-1       ; } // check if data stack has n items
boolean WB32V20::rsHasItems (int n) { return RP >= RS+n-1       ; } // check if return stack has n items
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::dsClear  () { DP=DS-1 ; } // reset data stack
void    WB32V20::dsPush   (int n) { T=n, *(++DP)=T; } // push a number onto data stack
int     WB32V20::dsPop    () { return *DP-- ; } // pop a number from data stack
int     WB32V20::dsDepth  () { return DP-DS+1; } // depth of data stack
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::rsClear  () { RP=RS-1 ; } // reset return stack
void    WB32V20::rsPush   (int n) { T=n, *(++RP)=T; } // push a number onto return stack
int     WB32V20::rsPop    () { return *RP-- ; } // pop a number from return stack
int     WB32V20::rsDepth  () { return RP-RS+1; } // depth of return stack
/////// ------------------------------- /////////////////////////////////////////////////////////////////
char    WB32V20::toDigit  (int x) { return x += x<10 ? 0x30 : 0x61-10; } // convert integer x into single digit
char*   WB32V20::toDigits (uint x, int b) { // convert integer x into digits in given base b
  char*p=tmp+TMP_SIZE; *(--p)='\0';
  if(x==0){ *(--p)='0'; return p; }
  boolean neg; if( neg=(b==10&&(int)x<0) ) x=-x;
  while(x){
    *(--p)=toDigit(x%b),x/=b; // convert current digit to character
  }
  if(neg) *(--p)='-';
  return p;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
Word**  WB32V20::adrExecWord () { return &W;     }; // get the forth word to execute
void    WB32V20::setExecWord (Word* w) { W=w;    }; // set the forth word to execute
Word*   WB32V20::getExecWord () { return W;      }; // get the forth word to execute
int*    WB32V20::adrBase () { return &B;         }; // get number convertion base B
void	  WB32V20::setBase (int b) { B=b;          }; // set b as number convertion base B
int		  WB32V20::getBase () { return B;          }; // get number convertion base B
int*    WB32V20::adrState () {return &state;     }; // get state COMPILING/INTERPRETING
void    WB32V20::setState (int s) { state=s;     }; // set s as state COMPILING/INTERPRETING
int     WB32V20::getState () {return state;      }; // get state COMPILING/INTERPRETING
int*    WB32V20::adrError () {return &error;     }; // get error id to see if error
void    WB32V20::setError (int e) { error=e;     }; // set non-zero e as error id
int     WB32V20::getError () {return error;      }; // get error id to see if error
int*    WB32V20::adrTracing () {return &tracing; }; // get depth of tracing
void    WB32V20::setTracing (int t) {tracing=t;  }; // set depth of tracing=t
int     WB32V20::getTracing () {return tracing;  }; // get depth of tracing
/////// ------------------------------- /////////////////////////////////////////////////////////////////
char*	  WB32V20::hexPrefix(char *s) { // 0xff 0XFF $ff $FF $Ff are all acceptable as hexadecimal numbers
  char c;
  if((c=*s++) != '0' && c != '$') return 0;
  if(c=='0' && (c=*s++) != 'x' && c != 'X') return 0;
  return s; // remain string
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
char*   WB32V20::uniqueString(char *s) { // 在 string buffer 備存字串 並回應 存放位址 p
  char *p=SB+1;                 // p is pointing to the first string
  while (p<sbEnd) {             // if p is not pointing to end of buffer
    if(!strcmp(p,s)) return p;  // return p if s is found in the string buffer
    p+=*(p-1);                  // p is pointing to the next string
  }
  int n=strlen(s);
  if (n > SL_LIMIT) {           // if s is longer than string length limit
    error=101;
    PRINTF("\"%s\" ???\nerror %d string length %d > %d",s,error,n,SL_LIMIT);
    return 0;
  }
  if(sbEnd+n >= SB_LIMIT){      // if string buffer has no more space to save s
    error=102;
    PRINTF("\"%s\" ???\nerror %d SB needs space %d over %d",s,error,sbEnd+n-SB,SB_LIMIT-SB);
    return 0;
  }
  *sbEnd++=(n+2);               // the gap count for going to next string
  strcpy(sbEnd,s);              // copy s to end of buffer
  p=sbEnd;                      // p is pointing to the the string copy
  sbEnd+=n+1;                   // advance the end of buffer
  return p;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
Word*   WB32V20::create(char*n, FuncP f){// create a forth word of name n with the function code f to execute
    char*u;
    Word *w=(Word*)malloc(sizeof(Word));
    Word *x=vocSearch(n);
    w->code=f;
    if(x){
      PRINTF(" ??? %s redef ???",n);
      u=x->name;
    } else {
      u=uniqueString(n); // get a unique string from sbuf
    }
    w->name=u;
    w->p.wpl=NULL;
    w->flag=0;
    return w;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::cpInit () { CP=CS;          } // initialize temporary compile space
void    WB32V20::compile (Word*w) { *CP++=w; } // compile w into temporary compile space
void    WB32V20::ipSet (Word**ip) { IP=ip;   } // set IP of wplist 
Word**  WB32V20::ipGet () { return IP;       } // get IP of wplist
/////// ------------------------------- /////////////////////////////////////////////////////////////////
Word**  WB32V20::wpClone() {              // cloning temporary compile space to a wplist
    int n=CP-CS, m=n*sizeof(Word*);
    Word** wplist=(Word**)malloc(m);
    memcpy(wplist,CS,m);
    return wplist;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
Voc*    WB32V20::vocGet() { return voc; } // return vocabulary
void    WB32V20::vocAdd (Word *w) { w->prev = voc->last; voc->last=w; } // add w to vocabulary
Word*	  WB32V20::vocSearch (char *name) { // search vocabulary for the forth word of given name
  Word *w=vocGet()->last;
  while ( w && strcmp(w->name,name) ) w=w->prev;
  return w;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::words(char*sub) { // show all word names having specific substring
  PRINTF("\n");
  int m=0, n;
  Word *w=vocGet()->last;
  while (w) { 
    if(!*sub || strstr(w->name,sub) ){
      n=strlen(w->name);
      if(m+n>CONSOLE_WIDTH) PRINTF("\n"), m=0;
      if(m)PRINTF(" "), m++;
      PRINTF(w->name), m+=n;
    }
    w=w->prev;
  }
  PRINTF("\n");
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::see(Word *w) { // show the forth word
  if(!w){ PRINTF(" ? undefinded "); return; }
  PRINTF("\n----------------------");
  PRINTF("\n%x prev %08x"            ,&w->prev ,w->prev        );
  PRINTF("\n%x flag %08x"            ,&w->flag ,w->flag        );
  PRINTF("\n%x type %08x %s"         ,&w->type ,w->type,w->type);
  PRINTF("\n%x name %08x %s"         ,&w->name ,w->name,w->name);
  PRINTF("\n%x code %08x"            ,&w->code ,w->code        );
  PRINTF("\n%x parm %08x"            ,&w->p    ,w->p           );
  PRINTF("\n----------------------\n");
  if ( !strcmp(w->type,"colon") ){
    Word**ip=w->p.wpl;
    Word*x;
    do {
       x=*ip;
       PRINTF("%x %08x %s%s\n",ip++,x,x->flag==IMMID_WORD?"[compile] ":"",x->name);
       if(!strcmp(x->name,"(lit)")){
          Word*z=*ip;
          PRINTF("%x %08x %d\n",ip++,z,z);
       }
    } while(strcmp(x->name,"(;)"));
    PRINTF("----------------------\n");
  }
  PRINTF("forth %s word %s\n"       , w->type, w->name        );
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::dump(int *a,int n) { // dump n cells at adr
    int *lmt=a+n;
    char *ba;
    char *blmt;
    for( ; a<lmt; a+=4) { 
      PRINTF("\n%8.8x : ", (int) a);
      for(int i=0; i< 4; i++){
        if( a+i>=lmt )PRINTF("         ");
        else          PRINTF("%8.8x ", *( a+i));
      }
      PRINTF(": ");
      ba=(char*)a, blmt=(char*)lmt;
      for(int i=0; i<16; i++){
        if(ba+i>=blmt)PRINTF("   ");
        else          PRINTF("%2.2x ", *(ba+i));
      }
      PRINTF(": ");
      for(int i=0; i<16; i++){
        if(ba+i>=blmt)PRINTF(" ");
        else {
          char c=*(ba+i); n=(int)c;
          if( n==0 ) c='.';
          else if( n<0x20 || (n>0x7e&&n<0xa4) || n>0xc6 ) c='_';
          else if(n>=0xa4&&n<=0xc6) { // head-byte of commmon big5
            n=(int)*(ba+i+1);
            if( n<0x40 || (n>0x7e&&n<0xa1) || n>0xfe) c='_'; // next is not tail-byte of commmon big5 
            else PRINTF("%c",c), c=(char)n, i++; // show head-byte of commmon big5 and get tail-byte
          }
          PRINTF("%c",c);
        }
      }
    }
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
boolean WB32V20::isColonWord (Word *w) {
  FuncP _doColon;
  if ( ! _doColon ) {
    _doColon = vocSearch("(:)")->code;
    PRINTF("while checking 0x%x %s definde _doColon = 0x%x\n",w,w->name,_doColon);
  }
  return W->code == _doColon;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::readLineBegin () {
  tBegin=tEnd=tib, tReady=true;
  pinMode(16,INPUT); PRINTF("\nled %d ",digitalRead(led)); pinMode(16,OUTPUT);
  PRINTF("LeftButtons ");
  PRINTF("LB %d LG %d LR %d LY %d ", digitalRead(LB), digitalRead(LG), digitalRead(LR), digitalRead(LY));
  PRINTF("RighttButtons ");
  PRINTF("RY %d RB %d ", digitalRead(RY), digitalRead(RB));
  PRINTF("BackButton ");
  PRINTF("PROG %d\n",digitalRead(PROG));
  dsShow();     // showing depth, numbers, and number coversion base of data stack 
  static int i=0; // setting index of input line
  PRINTF("--------------------------------------------------\n");
  PRINTF("line %02d >> ", i++); // asking for input line i
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::readLineEnd () {
  PRINTF("%s\n",tib);
  if ( tracing ) PRINTF("length %d tBegin 0x%x tEnd 0x%x",strlen(tib),tBegin,tEnd); // tib info
  *tEnd = 0;      // add null at end of tib
  tReady = false; // waiting to interpret tib
  interpret(tib); // begin to interpret tib
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::readLineContinue () {
  if ( ! tReady )           return;  // interpreting of last input might not finished
  if ( ! AVAILABLE() )        return;  // serial port not ready to read
  char c = READ();                     // read character from serial port
  if( error )                 return;  // press back side RESET button to 
  if ( c == '\b' ) {                   // back space
    if ( tEnd == tib )      return;     // just return if tib empty 
    PRINTF("\b \b");                      // erase last character
    tEnd--;                 return;     // ignore last character 
  }
  if ( c == '\r' || c == '\n' ) {      // if end of line
    readLineEnd();            return;  // interpretBegin
  }
  if ( tEnd >= tLimit ) {
    SHOW_MESSAGE("\n??? warning %03d tib $%x full ???\nbreak input at length %d\n",001,tib,tEnd-tib);
    error = 0;                         // ignore 
    readLineEnd();            return;
  }
  *tEnd++ = c;                       // push c into tib
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::init (Word *last) {
  for(int i=0; i<7; i++) pinMode(buttons[i],INPUT);  // setting pin mode direction of all the buttons as INPUT
  voc = (Voc*) malloc(sizeof(Voc));
  voc->last = last;   // initializing the link-list of 26 forth words as the forth vocabulary (dictionary)
  dsClear();          // clearing data stack
  rsClear();          // clearing return stack
  error = 0;
  PRINTF("\ntib 0x%x, tLimit 0x%x, TIB_SIZE %d\n",tib,tLimit,TIB_SIZE);
}
