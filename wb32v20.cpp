#include "wb32v20.h"
/////// --------------------------------- /////////////////////////////////////////////////////////////////
boolean WB32V20::EOL (char c)           { return c=='\n'||c=='\r'; } // check if c is end of line
boolean WB32V20::backSpace (char c)     { return c=='\b'; } // check if c is back space
boolean WB32V20::whiteSpace (char c)    { return c==' '||c=='\t'||c=='\n'||c=='\r'; } // check if c is white space
/////// --------------------------------- /////////////////////////////////////////////////////////////////
void WB32V20::traceIP(Word** ip){
  int depth=rsDepth();
  PRINTF("\ntracing %08x %08x ",ip,W);
  for(int i=1; i<depth/2; i++) PRINTF("| ");
}
void WB32V20::traceWord(Word** ip,Word* w){
  if(tracing){
    traceIP(ip);
    PRINTF("%s",W->name);
  }
}
void WB32V20::traceData(Word**ip,int n){
  if(tracing){
    traceIP(ip);
    PRINTF("%d",n);
  }
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
    showMessage("\n??? error %d ???\n%s undefined\n",101,token);
  }
  return n;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void	WB32V20::dsShow(){ // show data stack info
  PRINTF("\n< dsDepth %d [ ",dsDepth()); // show depth
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
  static Word *w_lit = 0, *w_ret = 0;
  int _doColon = 0;
  if( ! w_lit ) w_lit = vocSearch("(lit)");
  if( ! w_ret ) w_ret = vocSearch("(;)");
  if( ! _doColon ) _doColon = vocSearch("(:)")->p.con;
  if(!w){ PRINTF(" ? undefinded "); return; }
  PRINTF("\n----------------------");
  PRINTF("\n%x prev %08x"            ,&w->prev ,w->prev        );
  PRINTF("\n%x flag %08x"            ,&w->flag ,w->flag        );
  PRINTF("\n%x type %08x %s"         ,&w->type ,w->type,w->type);
  PRINTF("\n%x name %08x %s"         ,&w->name ,w->name,w->name);
  PRINTF("\n%x code %08x"            ,&w->code ,w->code        );
  PRINTF("\n%x parm %08x"            ,&w->p    ,w->p           );
  PRINTF("\n----------------------\n");
  if ( (int)w->code == _doColon ){
    Word**ip=w->p.wpl;
    Word*x;
    do {
       x=*ip;
       PRINTF("%x %08x %s%s\n",ip++,x,x->flag==IMMID_WORD?"[compile] ":"",x->name);
       if( x == w_lit ){
          Word*z=*ip;
          PRINTF("%x %08x %d\n",ip++,z,z);
       }
    } while ( x != w_ret );
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
  FuncP _doColon=0;
  if ( ! _doColon ) {
    _doColon = vocSearch("(:)")->code;
    if ( tracing ) PRINTF("forth word %s 0x%x code 0x%x\n",w->name,w,_doColon);
  }
  return W->code == _doColon;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::ms(int n){ waitTime=millis()+n; }  // waiting of n milli secons
/////// ------------------------------- /////////////////////////////////////////////////////////////////
char*    WB32V20::parseToken ()          { // use white spaces as delimiters to parse a token
  char c = *pEnd;
  while ( c && whiteSpace(c) ) c = *++pEnd; // ignore leading white spaces
  char *tokenBegin = pEnd; // set tokenBegin at non white space
  if ( c ) {
    while ( c && !whiteSpace(c) ) c = *++pEnd; // colect  non white spaces
    if ( c ) { // if not end of string
      int n=pEnd-tokenBegin;
      if ( n>TMP_SIZE-1 ) {
        showMessage("\n??? error %03d ???\nparsing token length %d > %d\n",101,n,TMP_SIZE-1);
        return tokenBegin;
      }
      strncpy(tmp,tokenBegin,n);
      tmp[n] = 0; // add null as the end of string
      tokenBegin = tmp;
    }
  }
  return tokenBegin;
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::call (Word**wplist)    { // inner interpreting the wplist of a forth colon word
  rsPush( (int)IP );
  IP = wplist;
  while(IP){
    W = *IP;
    traceWord(IP, W);
    W->code();
    if( IP ) IP++;
  }
  if ( tracing ) PRINTF("\n");
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::ipPush(){ rsPush((int)IP); rsPush((int)IP_head); }
void    WB32V20::ipPop (){ IP_head=(Word**)rsPop (); IP=(Word**)rsPop(); }
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::callBegin (Word*w) { // calling to the wplist of a forth colon word
  if ( tracing ) {
    PRINTF ("\ncalling %s 0x%x tReady %d pReady %d cReady %d\n",w->name,w,tReady,pReady,cReady);
  }
  ipPush();
  if ( tracing ) {
    PRINTF ("push IP 0x%x IP_head 0x%x to retur stack, tReady %d pReady %d cReady %d\n",IP,IP_head,tReady,pReady,cReady);
  }
  IP_head = IP = w->p.wpl, pReady = 0, cReady = 1;
  if ( tracing ) {
    PRINTF ("set IP 0x%x IP_head 0x%x, tReady %d pReady %d cReady %d\n",IP,IP_head,tReady,pReady,cReady);
  }
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::callContinue ()    { // continue calling the wplist of a forth colon word
  if ( ! digitalRead(LR) ) {                // press Left Red button to stop/resume parsing
    cReady=0;
    PRINTF("check callContinue 00 tReady %d pReady %d cReady %d since LR pressed\n",tReady,pReady,cReady);
  }
//PRINTF ("check callContinue 01 tReady %d pReady %d cReady %d\n",tReady,pReady,cReady);
  if ( ! cReady )             return; // calling might be stoped
  int t=millis();
  PRINTF ("check callContinue 02 tReady %d pReady %d cReady %d millis %08x waitTime %08x\n",tReady,pReady,cReady,t,waitTime);
  if ( t<waitTime )           return; // waiting asked by ms not finished 
  if ( ! IP ) {                       // end of calling
  //PRINTF ("check callContinue 03 tReady %d pReady %d cReady %d\n",tReady,pReady,cReady);
    if ( tracing ) PRINTF("\n");         // next line
    pReady = 1, cReady = 0;   return; // resume parsing 
  }
//PRINTF ("check callContinue 04 tReady %d pReady %d cReady %d IP 0x%x\n",tReady,pReady,cReady,IP);
  W=*IP;                              // from IP, get the forth word
//PRINTF ("check callContinue 05 tReady %d pReady %d cReady %d W  0x%x\n",tReady,pReady,cReady,W );
  traceWord(IP,W);                    // showing IP and W
//PRINTF ("check callContinue 06 tReady %d pReady %d cReady %d to   W->code 0x%x\n",tReady,pReady,cReady,W->code);
  W->code();                          // execute the forth word W
//PRINTF ("check callContinue 07 tReady %d pReady %d cReady %d from W->code 0x%x\n",tReady,pReady,cReady,W->code);
  if ( IP ) IP++;                     // IP point to next forth word
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::parseBegin (char *str) {
  if( ! tReady ) PRINTF("line %02d >> ", lineIndex++); // asking for input of lineIndex
  PRINTF("%s\n", str);
  int n=strlen(str);
  pBegin = pEnd = str, pLimit = str+n, pReady = 1, tReady = 0;
  if ( tracing ) PRINTF("\npEnd 0x%x pLimit 0x%x *pLimit 0x%02x len %d tReady %d pReady %d\n",pEnd,pLimit,*pLimit,n,tReady,pReady);
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::parseContinue() { // parse token to evaluate
  if ( ! digitalRead(LR) ) {                // press Left Red button to stop/resume parsing
    pReady=1-pReady, delay(100);
    if( tracing ) PRINTF("readLineContinue pReady %d since LR pressed\n",pReady);
  }
  if ( ! pReady )                   return; // input line not available
  if ( millis()<waitTime )          return; // waiting asked by ms not finished
  if ( IP )                         return; // calling forth colon word not finished
  if ( ! *pEnd ) {
    readLineBegin();                return; // end of parsing
  } // 
  char *token = parseToken();               // getting token
  W = vocSearch(token);                     // searching in vocabulary
  if( tracing ) PRINTF("token \"%s\" length %d word 0x%x state %d\n",token,strlen(token),W,state);
  if ( W ) {                                // if forth word W found
    if( state==INTERPRETING ||                 // state is INTERPRETING or
        W->flag==IMMID_WORD   ) {              // W is immediate
      if( isColonWord(W) ) {                      // W is forth Colon word
      	call(W->p.wpl);	return;
      //callBegin(W);        return;          // call forth colon word W // 待 debug !!!!!!!!
      } 
      W->code();                    return;       // else execute the non-colon word W
    }
    compile(W);                     return;    // state is COMPILING, compile W to temporary wplist
  }
  int n=toNumber(token);                    // if not a forth word, convert token to number
  if(error){                                // if not a valid number
    PRINTF("\n??? %s undefined ???\n");
    readLineBegin();                return;    // stop parsing
  }
  if(state==INTERPRETING){                  // if state is INTERPRETING
    dsPush(n);                      return;    // push number to data stack
  }
  static Word* wLit = 0;                    // wLit points to the forth word (lit)
  if( ! wLit ) {                            // if wLit not defined yet
    wLit = vocSearch("(lit)");                 // get the address of the forth word (lit)
    if(tracing) PRINTF("the forth word %s 0x%x\n",wLit->name,wLit);
  }
  compile(wLit), compile((Word*)n); return; // compile (lit) and the number n
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::readLineBegin () {
  tBegin=tEnd=tib, tReady=true, pReady=false, IP=0, waitTime=0;
  dsShow();     // showing depth, numbers, and number coversion base of data stack 
  if ( tracing ) {
    delay(1000);
    pinMode(16,INPUT); PRINTF("led %d ",digitalRead(led)); pinMode(16,OUTPUT);
    PRINTF("LeftButtons ");
    PRINTF("LB %d LG %d LR %d LY %d ", digitalRead(LB), digitalRead(LG), digitalRead(LR), digitalRead(LY));
    PRINTF("RighttButtons ");
    PRINTF("RY %d RB %d ", digitalRead(RY), digitalRead(RB));
    PRINTF("BackButton ");
    PRINTF("PROG %d\n",digitalRead(PROG));
  }
  PRINTF("--------------------------------------------------\n");
  PRINTF("line %02d >> ", lineIndex++); // asking for input of lineIndex
}
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::readLineContinue () {
  if ( ! tReady )           return;  // interpreting of last input might not finished
  if ( ! AVAILABLE() )      return;  // serial port not ready to read
  char c = READ();                   // read character from serial port
  if( error )               return;  // press back side RESET button to 
  if ( c == '\b' ) {                 // back space
    if ( tEnd == tib )      return;     // just return if tib empty 
    PRINTF("\b \b");                       // erase last character
    tEnd--;                 return;     // ignore last character 
  }
  if ( c == '\r' || c == '\n' ) {    // if end of line
    *tEnd = 0;                          // add null at end of tib
    parseBegin(tib);        return;     // interpretBegin
  }
  if ( tEnd >= tLimit ) {
    showMessage("\n??? warning %03d tib $%x full ???\nsplit input at length %d\n",001,tib,tEnd-tib);
    error = 0;                       // ignore 
    parseBegin(tib);        return;
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
/////// ------------------------------- /////////////////////////////////////////////////////////////////
void    WB32V20::toValue (Word *w,int n){ // store n to w if it is a forth value word
  if(tracing)PRINTF("\nstoring %d to the forth %s word %s \n ",n,w->type,w->name);
  if ( checking && strcmp(w->type,"value") ) { // w is not a forth value word
    showMessage("\n??? error %03d ???\ncannot store %d to non-value word %s ???\n",103,dsPop(),w->name);
    return;
  }
  if ( tracing ) PRINTF("\nstore %d to the forth word %s of type %s\n",n,w->name, w->type);
  *(w->p.val)=n;
}
