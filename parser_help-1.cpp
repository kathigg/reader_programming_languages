
#include<iostream>
#include<limits.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
using namespace std;

/*****************************************************************
 *                     DECLARATIONS                              *
 *****************************************************************/
typedef int NUMBER;
typedef int NAME;
const int  NAMELENG = 20;      /* Maximum length of a name */
const int  MAXNAMES = 100;     /* Maximum number of different names */
const int  MAXINPUT = 5000;     /* Maximum length of an input */
const char*   PROMPT = "-> ";
const char*   PROMPT2 = "> ";
const char  COMMENTCHAR = ';';
const int   TABCODE = 9;        /* in ASCII */

struct EXPLISTREC;
typedef EXPLISTREC* EXPLIST;
enum EXPTYPE {VALEXP,VAREXP,APEXP};
struct EXPREC
{
	  EXPTYPE etype; //what type of expression
	  NUMBER num;
	  NAME varble;
	  NAME optr;
	  EXPLIST args;
};
typedef EXPREC* EXP;

struct EXPLISTREC
{
	  EXP head;
	  EXPLIST tail;
};


struct VALUELISTREC
{
	  NUMBER  head;
	  VALUELISTREC*  tail;
};

typedef VALUELISTREC* VALUELIST;

struct NAMELISTREC
{
	  NAME   head;
	  NAMELISTREC* tail;
};
typedef NAMELISTREC* NAMELIST;

struct  ENVREC
{
	   NAMELIST vars;
	   VALUELIST values;
};

typedef ENVREC* ENV;

struct  FUNDEFREC
{
	   NAME  funname;
	   NAMELIST  formals;
	   EXP  body;
	   FUNDEFREC*  nextfundef;
};
typedef FUNDEFREC* FUNDEF;

FUNDEF  fundefs;

ENV globalEnv;

EXP currentExp;

char userinput[MAXINPUT];
int   inputleng, pos;

char*   printNames[MAXNAMES];
int   numNames, numBuiltins;

int   quittingtime;



/*****************************************************************
 *                     DATA STRUCTURE OP'S                       *
 *****************************************************************/

/* mkVALEXP - return an EXP of type VALEXP with num n            */

EXP mkVALEXP ( NUMBER n)
{
   EXP e;
   e = new EXPREC;
   e->etype = VALEXP;
   e->num = n;
   return e;
}/* mkVALEXP */


/* mkVAREXP - return an EXP of type VAREXP with varble nm  */

EXP mkVAREXP ( NAME nm)
{
   EXP e;
    e = new EXPREC;
    e->etype = VAREXP;
    e->varble = nm;
   return e;
}/* mkVAREXP */


/* mkAPEXP - return EXP of type APEXP w/ optr op and args el     */

EXP mkAPEXP (NAME op, EXPLIST el)
{
   EXP e = new EXPREC;
   e->etype = APEXP;
   e->optr = op;
   e->args = el; 
   return e; 
}/* mkAPEXP */

/* mkExplist - return an EXPLIST with head e and tail el         */

EXPLIST mkExplist (EXP e, EXPLIST el)
{
    EXPLIST newel = new EXPLISTREC;
    newel->head = e;
    newel->tail = el;
    return newel;
}/* mkExplist */

/* mkNamelist - return a NAMELIST with head nm and tail nl        */

NAMELIST mkNamelist ( NAME nm, NAMELIST nl)
{
   NAMELIST newnl = new NAMELISTREC; 
   newnl->head = nm;
   newnl->tail = nl;
   return newnl;
}/* mkNamelist */

/* mkValuelist - return an VALUELIST with head n and tail vl     */

VALUELIST mkValuelist (NUMBER n,  VALUELIST vl)
{
   VALUELIST newvl = new VALUELISTREC;
   newvl->head = n;
   newvl->tail = vl; 
   return newvl;
}/* mkValuelist */

/* mkEnv - return an ENV with vars nl and values vl              */

ENV mkEnv ( NAMELIST nl, VALUELIST vl)
{
    ENV rho = new ENVREC;
    rho->vars = nl;
    rho->values = vl; 
    return rho;
}/* mkEnv */

/* lengthVL - return length of VALUELIST vl      */

int lengthVL ( VALUELIST vl)
{
   int i = 0;
   while (vl != 0)
   {
	 i++;
	 vl = vl->tail;
   }
   return i;
}/* lengthVL */

/* lengthNL - return length of NAMELIST nl    */

int lengthNL ( NAMELIST nl)
{
   int count = 0;
   while (nl != nullptr) {
      ++count; 
      nl = nl->tail; 
   }
   return count; 
}/* lengthNL */

/*****************************************************************
 *                     NAME MANAGEMENT                           *
 *****************************************************************/

/* fetchFun - get function definition of fname from fundefs */

FUNDEF fetchFun ( NAME fname)
{
   FUNDEF  f = fundefs; 
   while (f != nullptr) {
      if (f->funname == fname) return f; 
      f = f->nextfundef;
   }
   return nullptr; // returns nullptr if no definition has the name
}/* fetchFun */


/* newFunDef - add new function fname w/ parameters nl, body e   */
void  newFunDef (NAME fname,  NAMELIST nl, EXP e)
{
   FUNDEF f;
   f = fetchFun(fname);
   if (f == 0) /* fname not yet defined as a function */
   {
	 f = new FUNDEFREC;
	 f->nextfundef = fundefs; // place new FUNDEFREC
	 fundefs = f;        // on fundefs list
   }
   f->funname = fname;
   f->formals = nl;
   f->body = e;
}// newFunDef


/* initNames - place all pre-defined names into printNames */

void initNames()
{
   int i =0;
   fundefs = 0;
   printNames[i] = (char* )"loop";   i++; //0
   printNames[i] = (char* )"if";      i++; //1
   printNames[i] = (char* )"block";    i++; //2
   printNames[i] = (char*)"set";      i++; //3
   printNames[i] = (char* )"+";       i++; //4
   printNames[i] = (char* )"-";       i++; //5
   printNames[i] = (char* ) "*";       i++; //6
   printNames[i] = (char* )"/";       i++; //7
   printNames[i] = (char* )"=";       i++; //8
   printNames[i] = (char* )"<";       i++; //9
   printNames[i] = (char* )">";       i++; //10
   printNames[i] = (char* )"print"; // 11
   numNames = i;
   numBuiltins = i;
}//initNames

/* install - insert new name into printNames  */

NAME install ( char* nm)
{
   int i = 0;
   while (i <= numNames) {
	 if (strcmp( nm,printNames[i] ) == 0)
	    break;
     i++;
   }
   if (i > numNames)
   {
	  if (i >= MAXNAMES)
	  {
	     cout << "Error: symbol table is full" << endl;
	     exit(1);
	  }
	  numNames = i;
	  printNames[i] = new char[strlen(nm) + 1];
	  strcpy(printNames[i], nm);
   }
   return i;
}// install

/* prName - print name nm              */

void prName ( NAME nm)
{
	 cout<< printNames[nm];
} //prName

/*****************************************************************
 *                        INPUT                                  *
 *****************************************************************/

/* isDelim - check if c is a delimiter   */

int isDelim (char c)
{
   return ( ( c == '(') || ( c == ')') ||( c == ' ')||( c== COMMENTCHAR) );
}

/* skipblanks - return next non-blank position in userinput */

int skipblanks (int p)
{
   while (userinput[p] == ' ')
	++p;
   return p;
}


/* matches - check if string nm matches userinput[s .. s+leng]   */

int matches (int s, int leng,  char* nm)
{
   if (s < 0 || s + leng > inputleng + 1)
      return 0;
   int i=0;
   while (i < leng )
   {
	 if( userinput[s] != nm[i] )
	    return 0;
	 ++i;
	 ++s;
    }
   if (!isDelim(userinput[s]) )
	  return 0;
   return 1;
}/* matches */



/* nextchar - read next char - filter tabs and comments */

bool nextchar (char& c)
{
    if (scanf("%c", &c) != 1)
       return false;
    if (c == COMMENTCHAR )
    {
	  while ( c != '\n' )
		if (scanf("%c",&c) != 1)
		   return false;
    }
    return true;
}


/* readParens - read char's, ignoring newlines, to matching ')' */
void readParens()
{
   int parencnt; /* current depth of parentheses */
   char c = '\0';
   parencnt = 1; // '(' just read
   do
   {
	  if (c == '\n')
	    cout <<PROMPT2;
	  cout.flush();
	  if (!nextchar(c))
	  {
	     cout << "Error: expected ')' before end of input" << endl;
	     exit(1);
	  }
	  pos++;
	  if (pos == MAXINPUT )
	  {
		cout <<"User input too long\n";
		exit(1);
	  }
	  if (c == '\n' )
		userinput[pos] = ' ';
	  else
		userinput[pos] = c;
	  if (c == '(')
		++parencnt;
	  if (c == ')')
	    parencnt--;
	}
    while (parencnt != 0 );
} //readParens

/* readInput - read char's into userinput */

bool readInput()
{
    char  c;
    cout << PROMPT;
    cout.flush();
    pos = -1;
    do
	 {
	    if (!nextchar(c))
	    {
	       if (pos == -1)
	          return false;
	       c = '\n';
	    }
	    ++pos ;
	    if (pos >= MAXINPUT - 1)
	    {
		    cout << "User input too long\n";
		    exit(1);
	    }
	    if (c == '\n' )
		   userinput[pos] = ' ';
	    else
		   userinput[pos] = c;
	    if (userinput[pos] == '(' )
		  readParens();
	 }
	while (c != '\n');
	inputleng = pos;
	userinput[pos+1] = COMMENTCHAR; // sentinel
   return true;
}


/* reader - read char's into userinput; be sure input not blank  */

bool reader ()
{
    do
    {
	  if (!readInput())
	     return false;
	  pos = skipblanks(0);
    }
    while( pos > inputleng); // ignore blank lines
    return true;
}

/* parseName - return (installed) NAME starting at userinput[pos]*/

NAME parseName()
{
   char nm[NAMELENG + 1]; // array to accumulate characters
   int leng; // length of name
   leng = 0;
   while ( (pos <= inputleng) && !isDelim(userinput[pos]) )
   {
	    if (leng == NAMELENG)
	    {
	       cout << "Error: name is too long" << endl;
	       exit(1);
	    }
	    nm[leng] = userinput[pos];
	    ++pos;
	    ++leng;
   }
   if (leng == 0)
   {
	   cout<<"Error: expected name, instead read: "<< userinput[pos]<<endl;
	   exit(1);
   }
   nm[leng] = '\0';
   pos = skipblanks(pos); // skip blanks after name
  
   return ( install(nm) );
}// parseName

/* isDigits - check if sequence of digits begins at pos   */

int isDigits (int p)
{
   return p <= inputleng && userinput[p] >= '0' && userinput[p] <= '9';
}// isDigits


/* isNumber - check if a number begins at pos  */

int isNumber (int p)
{
   return isDigits(p) ||
          (p <= inputleng && userinput[p] == '-' && isDigits(p + 1));
}// isNumber

/* parseVal - return number starting at userinput[pos]   */

NUMBER parseVal()
{
   int sign = 1;
   long long value = 0;
   if (userinput[pos] == '-'){
      sign = -1;
      ++pos;
   }
   if (pos > inputleng || userinput[pos] < '0' ||
      userinput[pos] > '9') {
         cout << "Error: expected digits in number, instead read : " << userinput[pos] << endl;
         exit(1); 
   }
   long long limit = sign < 0 ? -static_cast<long long>(INT_MIN) : INT_MAX;
   while (pos <= inputleng && userinput[pos] >= '0' && userinput[pos] <= '9') {
      int digit = userinput[pos] - '0';
      if (value > (limit - digit) / 10) {
         cout << "Error: number is out of range" << endl;
         exit(1);
      }
      value = value * 10 + digit;
      ++pos; 
   }
   if (!isDelim(userinput[pos])) {
      cout << "Error: invalid character after number: " << userinput[pos] << endl;
      exit(1);
   }
   pos = skipblanks(pos); 
   return static_cast<NUMBER>(sign * value);
}// parseVal

EXPLIST parseEL();

/* parseExp - return EXP starting at userinput[pos]  */

EXP parseExp()
{
   NAME nm;
   EXPLIST el;
   if ( userinput[pos] == '(' )
   {// APEXP
      pos = skipblanks(pos+1); // skip '( ..'
      nm = parseName();
      el = parseEL();
      return ( mkAPEXP(nm, el));
   }
   if (isNumber(pos))
      return ( mkVALEXP((NUMBER)parseVal() ));  // VALEXP
   return ( mkVAREXP((NAME)parseName() ) ); // VAREXP
}// parseExp

/* parseEL - return EXPLIST starting at userinput[pos]  */

EXPLIST parseEL()
{
   EXP e;
   EXPLIST el;
   if ( userinput[pos] == ')')
   {
     pos = skipblanks(pos+1); // skip ') ..'
     return 0;
   }
   e = parseExp();
   el = parseEL();
   return ( mkExplist(e, el));
}// parseEL


/* parseNL - return NAMELIST starting at userinput[pos]  */

NAMELIST parseNL()
{
    NAMELIST nl = nullptr; 
    NAMELISTREC* last = nullptr; 
    while (pos <= inputleng && userinput[pos] != ')'){
      NAME nm = parseName(); // returning the name's symbol-table location 
      NAMELISTREC* node = new NAMELISTREC{nm, nullptr};
      if (nl == nullptr) nl = node; // happens for the first node
      else last->tail = node; 
      last = node; 
    }
    if (pos > inputleng) {
       cout << "Error: expected ')' after argument list" << endl;
       exit(1);
    }
    pos = skipblanks(pos + 1);
    return nl;
}// parseNL

/* parseDef - parse function definition at userinput[pos]   */

NAME parseDef()
{
   NAME fname;        // function name
   NAMELIST nl;       // formal parameters
   EXP e;             // body
   // skip blanks, skip ( define
   if (userinput[pos] != '(') {
      cout << "Error: expected '(' before definition" << endl;
      exit(1);
   }
   pos = skipblanks(pos + 1); // skip the outer (
   if (!matches(pos, 6, (char*)"define")) {
      cout << "Error: expected define, instead read: " << userinput[pos] << endl; 
      exit(1); 
   }
   pos = skipblanks(pos + 6); // skip define and blanks
   // then you get the name fname
   fname = parseName();
   // then you skip blanks again, skip the left
   // parenthesis, grab the function
   if (userinput[pos] != '(') {
      cout << "Error: expected '(' before the argument list, instead read: " << userinput[pos] << endl;
      exit(1);
   }
   pos = skipblanks(pos + 1); // skip ( before the arguments
   // get nl by calling parseNL
   nl = parseNL();
   // then you parse the expression, skip blanks, get e
   e = parseExp();
   if (userinput[pos] != ')') {
      cout << "Error: expected ')' to end the definition, but instead read: " << userinput[pos] << endl;
      exit(1); 
   }
   pos = skipblanks(pos + 1); // skipping the outer ')'
   newFunDef(fname, nl, e);
   // parsing means the entire function has to be consumed.
   return fname;
}// parseDef

/* 
NUMBER eval (EXP e, ENV rho) 
{ 
switch (e->etype) {
case VALEXP: return (e->num);
vase VAREXP:// do this --- if it's not local, look up global, if it's not global than crash.
case APEXP: if (e->optr > numBuiltIns) 
   return applyUserFun(e->optr, evalList(e->args, rho)); 
   else { 
   if (e->optr<4) 
   return applyCtrolOp(e->optr, e->args, rho); 
   return applyValueOp(e->optr, evalList(e->args, rho));  
   // evalList goes and evaluates each item in the list and makes a linkedlist, 
   // and gives you a pointer to it. It evaluates this recursively. It calls eval recursively, and it calls it back, and vice versa. 
   // function looks like (f 3 4 5) (as written on the board, a bit haphazardly) 
}
} 
return 0; 
} // eval 

/*****************************************************************
 *                     ENVIRONMENTS                              *
 *****************************************************************/

/* emptyEnv - return an environment with no bindings */

ENV emptyEnv()
{
   return  mkEnv(0, 0);
}
/*
/* bindVar - bind variable nm to value n in environment rho */ 
/* 
void bindVar(NAME n,, NUMBER n, ENV rho)  {
rho->vars = mkNameList(nm, rho->vars); //mkNameList make s node, initializes nm at the start of the linkedlist
rho->values = mkValueList(n, rho->values);
}
// findVar -- look up 
VALUELIST findVar(NAME nm, ENV rho) {}

void assign (NAME nm, NUMBER n, ENV rho) {
VALUELIST varloc; as soon as you find its value you change its value to n. 
}
// fetch returns number
NUMBER fetch (NAME nm, ENV rho) { // you return a number 
VALUELIST vl; 
}

// isBound -- check if nm is bound in rho, returns a true/false boolean 
int isBound(NAME nm, ENV rho) {} 
*/

/*******NUMBERS*********/
// prValue - print number n 
void prValue (NUMBER n) {
   cout << n;
} // pr value 
int isTrueVal(NUMBER n){
   return (n!=0); 
} // is true vale

NUMBER applyValueOp(int o, VALUELIST vl) {
   NUMBER n, n1, n2;
   // more stuff 
}
/********EVALUATION
 * NUMBER eval (Exp e, ENV rho); 
 * VALUELIST evalList(EXPLIST el; ENV rho) {
 * NUMBER h; // the head. how do you compute the head? 
 * VALUELIST t; 
 * if (el == 0) 
 * return 0; 
 * h = eval(el->head, rho); 
 * t = evalList(el-> tail, rho); 
 * 
 * // applyUserFun == look up definition of nm and apply to actuals 
 * NUMBER applyUserFun(NAME nm, VALUELIST actuals) {
 * FUNDEF f;
 * ENV rho;  

/*****************************************************************
 *                     READ-EVAL-PRINT LOOP                      *
 *****************************************************************/

int main()
{
   
   initNames();
   globalEnv = emptyEnv();

   quittingtime = 0;
   while (!quittingtime)
   {
      

	 if (!reader())
	    break;
	 if ( matches(pos, 4, (char* )"quit"))
	    quittingtime = 1;
	 else if( (userinput[pos] == '(') &&
		    matches(skipblanks(pos+1), 6, (char* )"define")  )
	 {
		    prName(parseDef());
		    cout <<endl;
	 }
	 else {
			currentExp = parseExp();
			// prValue(eval(currentExp, emptyEnv() )); // UNCOMMENTED IN MAHE CODE
			cout <<endl<<endl;
		 }
	 if (!quittingtime && pos <= inputleng)
	 {
	    cout << "Error: unexpected input after expression: " << userinput[pos] << endl;
	    exit(1);
	 }
	}// while
    return 0;
}


