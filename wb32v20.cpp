#include "wb32v10.h"
/////// --------------------------------- /////////////////////////////////////////////////////////////////
boolean WB32V10::EOL (char c)           { return c=='\n'||c=='\r'; } // check if c is end of line
boolean WB32V10::backSpace (char c)     { return c=='\b'; } // check if c is back space
boolean WB32V10::whiteSpace (char c)    { return c==' '||c=='\t'||c=='\n'||c=='\r'; } // check if c is white space
/////// --------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V10::tibOpen ()             { tibBegin=tibEnd=tib,tibLimit=tib+TIB_SIZE-1; } // init tib and wait for input
void    WB32V10::tibClose ()            { *tibEnd='\0'; } // add '\0' at end of input
void    WB32V10::tibPop ()              { --tibEnd; } // pop last input character
void    WB32V10::tibPush (char c)       { *(tibEnd++)=c; } // collect input character
boolean WB32V10::tibEmpty ()            { return tibEnd==tibBegin; } // check if buffer is empty
boolean WB32V10::tibFull ()             { return tibEnd==tibLimit; } // check if buffer is full
/////// --------------------------------- /////////////////////////////////////////////////////////////////
void	  WB32V10::waitInput ()           {  while ( !AVAILABLE() ); } // read input characters until end of line
char*   WB32V10::readLine ()            { // read input characters until end of line
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
void    WB32V10::parseBegin (char *str) { parseRemain = tibBegin = str; tibEnd = tibBegin+strlen(str); }
char    WB32V10::parseAvailable ()      { return *parseRemain; } // non '\0' means available to parse
char*	  WB32V10::parseToken ()          { // use white spaces as delimiters to parse a token
  char c = *parseRemain;
  while ( c && whiteSpace(c) ) c = *++parseRemain; // ignore leading white spaces
  char *start = parseRemain; // set start at non white space
  if ( c ) {
    while ( c && !whiteSpace(c) ) c = *++parseRemain; // colect  non white spaces
    char *limit = parseRemain; // set limit at white space
    if ( c ) { // if not end of string
      int n=limit-start;
      if ( n>TMP_SIZE-1 ) { //
      	PRINTF("\n error WB32V10::parseToken length %d > %d\n",n,TMP_SIZE-1);
      	return start;
      }
      strncpy(tmp,start,n);
      tmp[n] = '\0'; // add null as the end of string
      start = tmp;
    }
  }
  return start;
}
void WB32V10::traceIP(Word** ip){
  int depth=rsDepth();
  PRINTF("tracing %08x %08x ",ip,W);
  for(int i=1; i<depth; i++) PRINTF("| ");
}
void WB32V10::traceWord(Word** ip,Word* w){
  if(tracing){
    traceIP(ip);
    PRINTF("%s\n",W->name);
  }
}
void WB32V10::traceData(Word**ip,int n){
  if(tracing){
    traceIP(ip);
    PRINTF("%d\n",n);
  }
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V10::call (Word**wplist)    { // inner interpreting the wplist of a forth colon word
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
void	  WB32V10::interpret (char *line) { // interpreting given string
  parseBegin(line);
  while ( parseAvailable() ) {
    char *token = parseToken();
    eval(token);
  }
  PRINTF("\n");
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
int	WB32V10::toNumber(char *token){
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
void WB32V10::eval(char *token){
  char *remain, *p=token, c;
  W=vocSearch(token);
  Word* wLit;
  if(!wLit) wLit=vocSearch("(lit)");
  if(W){
    if(state==INTERPRETING || W->flag==IMMID_WORD){
      IP=0;
      W->code();
      return;
    } else {
      wpPush(W); 
    }
  } else {
    int n=toNumber(token);
    if(!getError()){
      if(state==INTERPRETING){
        dsPush(n);
      } else {
        wpPush(wLit);
        wpPush((Word*)n);
      }
    }
  }
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void	WB32V10::dsShow(){ // show data stack info
  PRINTF("< dsDepth %d [ ",dsDepth()); // show depth
  if(dsDepth()>0){
    if(dsDepth()>5)PRINTF(".. "); // showing at most top 5 items
    for ( int *i=max(DP-4,DS); i<=DP; i++ ) PRINTF("%s ",toDigits(*i,B)); // show data
  }
  PRINTF("] base%d >\n",B); // show base 
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
boolean WB32V10::dsFull     ()      { return DP   >= DS+DS_DEPTH; } // check if data stack full
boolean WB32V10::dsHasSpace (int n) { return DP+n <= DS+DS_DEPTH; } // check if data stack has space for n items
boolean WB32V10::dsHasItems (int n) { return DP >= DS+n-1       ; } // check if data stack has n items
boolean WB32V10::rsHasItems (int n) { return RP >= RS+n-1       ; } // check if return stack has n items
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V10::dsClear  () { DP=DS-1 ; } // reset data stack
void    WB32V10::dsPush   (int n) { T=n, *(++DP)=T; } // push a number onto data stack
int     WB32V10::dsPop    () { return *DP-- ; } // pop a number from data stack
int     WB32V10::dsDepth  () { return DP-DS+1; } // depth of data stack
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V10::rsClear  () { RP=RS-1 ; } // reset return stack
void    WB32V10::rsPush   (int n) { T=n, *(++RP)=T; } // push a number onto return stack
int     WB32V10::rsPop    () { return *RP-- ; } // pop a number from return stack
int     WB32V10::rsDepth  () { return RP-RS+1; } // depth of return stack
/////// ------------------------------- /////////////////////////////////////////////////////////////////
char    WB32V10::toDigit  (int x) { return x += x<10 ? 0x30 : 0x61-10; } // convert integer x into single digit
char*   WB32V10::toDigits (uint x, int b) { // convert integer x into digits in given base b
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
Word**  WB32V10::adrExecWord () { return &W;     }; // get the forth word to execute
void    WB32V10::setExecWord (Word* w) { W=w;    }; // set the forth word to execute
Word*   WB32V10::getExecWord () { return W;      }; // get the forth word to execute
int*    WB32V10::adrBase () { return &B;         }; // get number convertion base B
void	  WB32V10::setBase (int b) { B=b;          }; // set b as number convertion base B
int		  WB32V10::getBase () { return B;          }; // get number convertion base B
int*    WB32V10::adrState () {return &state;     }; // get state COMPILING/INTERPRETING
void    WB32V10::setState (int s) { state=s;     }; // set s as state COMPILING/INTERPRETING
int     WB32V10::getState () {return state;      }; // get state COMPILING/INTERPRETING
int*    WB32V10::adrError () {return &error;     }; // get error id to see if error
void    WB32V10::setError (int e) { error=e;     }; // set non-zero e as error id
int     WB32V10::getError () {return error;      }; // get error id to see if error
int*    WB32V10::adrTracing () {return &tracing; }; // get depth of tracing
void    WB32V10::setTracing (int t) {tracing=t;  }; // set depth of tracing=t
int     WB32V10::getTracing () {return tracing;  }; // get depth of tracing
/////// ------------------------------- /////////////////////////////////////////////////////////////////
char*	  WB32V10::hexPrefix(char *s) { // 0xff 0XFF $ff $FF $Ff are all acceptable as hexadecimal numbers
  char c;
  if((c=*s++) != '0' && c != '$') return 0;
  if(c=='0' && (c=*s++) != 'x' && c != 'X') return 0;
  return s; // remain string
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
char*   WB32V10::uniqueString(char *s) { // 在 string buffer 備存字串 並回應 存放位址 p
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
Word*   WB32V10::create(char*n, FuncP f){// create a forth word of name n with the function code f to execute
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
void    WB32V10::wpPush (Word*w) { *CP++=w; } // push wp into temporary wplist
void    WB32V10::cpInit () { CP=CS;         } // initialize tmp wplist
void    WB32V10::ipSet (Word**ip) { IP=ip;  } // set IP of wplist
Word**  WB32V10::ipGet () { return IP;      } // get IP of wplist
Word*   WB32V10::wpNext() { return *IP++;   } // get next wp and increase IP
/////// ------------------------------- /////////////////////////////////////////////////////////////////
Word**  WB32V10::wpClone() {              // cloning temporary compile space to a wplist
    int n=CP-CS, m=n*sizeof(Word*);
    Word** wplist=(Word**)malloc(m);
    memcpy(wplist,CS,m);
    return wplist;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
Voc*    WB32V10::vocGet() { return voc; } // return vocabulary
void    WB32V10::vocAdd (Word *w) { w->prev = voc->last; voc->last=w; } // add w to vocabulary
Word*	  WB32V10::vocSearch (char *name) { // search vocabulary for the forth word of given name
  Word *w=vocGet()->last;
  while ( w && strcmp(w->name,name) ) w=w->prev;
  return w;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V10::words(char*sub) { // show all word names having specific substring
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
void    WB32V10::see(Word *w) { // show the forth word
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
void    WB32V10::dump(int *a,int n) { // dump n cells at adr
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
void    WB32V10::init (Word *last) {
  voc = (Voc*) malloc(sizeof(Voc));
  voc->last = last;   // initializing the link-list of 26 forth words as the forth vocabulary (dictionary)
  dsClear();          // clearing data stack
  rsClear();          // clearing return stack
}
