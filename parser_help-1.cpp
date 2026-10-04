
#include<iostream>
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
   printNames[i] = (char* )"loop";   i++;
   printNames[i] = (char* )"if";      i++;
   printNames[i] = (char* )"block";    i++;
   printNames[i] = (char*)"set";      i++;
   printNames[i] = (char* )"+";       i++;
   printNames[i] = (char* )"-";       i++;
   printNames[i] = (char* ) "*";       i++;
   printNames[i] = (char* )"/";       i++;
   printNames[i] = (char* )"=";       i++;
   printNames[i] = (char* )"<";       i++;
   printNames[i] = (char* )">";       i++;
   printNames[i] = (char* )"print";
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

void nextchar (char& c)
{
    scanf("%c", &c);
    if (c == COMMENTCHAR )
    {
	  while ( c != '\n' )
		scanf("%c",&c);
    }
}


/* readParens - read char's, ignoring newlines, to matching ')' */
void readParens()
{
   int parencnt; /* current depth of parentheses */
   char c;
   parencnt = 1; // '(' just read
   do
   {
	  if (c == '\n')
	    cout <<PROMPT2;
	  cout.flush();
	  nextchar(c);
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

void readInput()
{
    char  c;
    cout << PROMPT;
    cout.flush();
    pos = -1;
    do
	 {
	    ++pos ;
	    if (pos == MAXINPUT )
	    {
		    cout << "User input too long\n";
		    exit(1);
	    }
	    nextchar(c);
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
}


/* reader - read char's into userinput; be sure input not blank  */

void reader ()
{
    do
    {
	  readInput();
	  pos = skipblanks(0);
    }
    while( pos > inputleng); // ignore blank lines
}

/* parseName - return (installed) NAME starting at userinput[pos]*/

NAME parseName()
{
   char nm[20]; // array to accumulate characters
   int leng; // length of name
   leng = 0;
   while ( (pos <= inputleng) && !isDelim(userinput[pos]) )
   {
	    
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

int isDigits (int pos)
{
   
}// isDigits


/* isNumber - check if a number begins at pos  */

int isNumber (int pos)
{
   if (pos > inputleng) return 0;
   if (userinput[pos] >= '0' && userinput[pos] <= '9') {
      return 1; 
   return userinput[pos] == '-' && pos + 1 <= inputleng && userinput[pos+1] >= '0' && userinput[pos+1] <= '9'; 
   }
}// isNumber

/* parseVal - return number starting at userinput[pos]   */

NUMBER parseVal()
{
   int sign = 1;
   NUMBER value = 0; 
   if (userinput[pos] == '-'){ 
      sign = -1;
      ++pos;
   }
   if (pos > inputleng || userinput[pos] < '0' || 
      userinput[pos] > '9') {
         cout << "Error: expected digits in number, instead read : " << userinput[pos] << endl;
         exit(1); 
   }
   while (pos <= inputleng && userinput[pos] >= '0' && userinput[pos] <= '9') {
      value = value * 10 + (userinput[pos] - '0');
      ++pos; 
   }
   pos = skipblanks(pos); 
   return sign * value; 
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
    while (userinput[pos] != ')'){ 
      NAME nm = parseName(); // returning the name's symbol-table location 
      NAMELISTREC* node = new NAMELISTREC{nm, nullptr};
      if (nl == nullptr) nl = node; // happens for the first node
      else last->tail = node; 
      last = node; 
    }
    return nl;
}// parseNL

/* parseDef - parse function definition at userinput[pos]   */

NAME parseDef()
{
    NAME fname;        // function name
    NAMELIST nl;       // formal parameters
    EXP e;             // body
   // skip blanks, skip ( define 
   pos = skipblanks(pos + 1); // skip the outer (
   if (!matches(pos, 6, (char*)"define")) { 
      cout << "Error: expected define, instead read: " << userinput[pos] << endl; 
      exit(1); 
   }
   pos = skipblanks(pos + 6); // skip define and blanks
    // then you get the name fname 
    NAME fname = parseName();
    // then you skip blanks again, skip the left
    // parenthesis, grab the function 
    // get nl by calling parseNL
    NAMELIST nl = parseNL(); 
    // then you parse the expression, skip blanks, get e
    if (userinput[pos] != '(') {
      cout << "Error: expected '(' before the argument list, instead read: " << userinput[pos] << endl;
      exit(1);
    }
    pos = skipblanks(pos+1); // skip ( before the arguments
    NAMELIST args = parseNL();
    EXP body = parseExp(); 
    if (userinput[pos] != ')') {
      cout << "Error: expected ')' to end the definition, but instead read: " << userinput[pos] << endl;
      exit(1); 
    }
    pos = skipblanks(pos+1); // skipping the outer ')'
    newFunDef(fname, args, body); 
    // parsing means the entire function has to be consumed. 
    return (fname); 

   int functionNameLocation = parseName(); 
   return ( fname);
}// parseDef

/*****************************************************************
 *                     ENVIRONMENTS                              *
 *****************************************************************/

/* emptyEnv - return an environment with no bindings */

ENV emptyEnv()
{
   return  mkEnv(0, 0);
}

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
   	
	 reader();
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
			//prValue(eval(currentExp, emptyEnv() ));
			cout <<endl<<endl;
		 }
	}// while
    return 0;
}


