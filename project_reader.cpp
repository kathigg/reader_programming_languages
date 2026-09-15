// for the READER project, programming languages 
#include <iostream>
#include <stack>
using namespace std; 

typedef int NUMBER;
typedef int NAME;
const int NAMELENG = 20;
const int MAXNAMES = 100;
const int MAXINPUT = 5000;
const char* PROMPT = "-->  ";
const char* PROMPT2 = ">  ";
const char COMMENTCHAR = ';';
const int TABCODE = 9; // in ASCII, may or may not be right

struct EXPLISTREC;
typedef EXPLISTREC* EXPLIST;
enum EXPTYPE { VALEXP, VAREXP, APEXP};
enum EXPTYPE { VALEXP, VAREXP, APEXP };
struct EXPREC {
    EXPTYPE etype; // what type of expression
    NUMBER num;
    NAME variable; 
};
struct EXPLISTREC {
    EXP head; 
    EXPLIST tail;
};
struct VALUELISTREC {
    NUMBER head; 
    VALUELISTREC* tail;
    typedef VALUELISTREC* VALUELIST;
};
struct NAMELISTREC {
    NAME head; 
    NAMELISTREC* tail;
    typedef NAMELISTREC* NAMELIST;
};
struct ENVREC {
    NAMELIST vars;
    VALUELIST values;
};
typedef ENVREC* ENV; 
struct FUNDEFREC {
    NAME funname; 
    NAMELIST formals;
    EXP body; 
    FUNDREC* nextfundef; 
}; 
typedef FUNDEFREC* FUNDEF; 
FUNDEF fundefs; 
ENV globalEnv; 
EXP currentExp; 

// ...

EXP mkVALEXP(NUMBER n) {
    EXP e;
    e = new EXPREC;
    e -> etype = VALEXP;
    e -> num = n; 
    return e; 
} 

// ...

int lengthVL(VALUELIST vl){
    int i = 0;
    while (vl != 0){
        i++; 
        vl = vl -> tail;
    }
    return i; 
}

int lengthML(NAME fname) {
    FUNDEF f;
    f = fundefs; 
} 

// void newFunDef(NAME fname; NAMELIST nl; EXP e);

// init names -- place all pre-defined names ino printNames 
void initNames() {
    int i = 0; 
    fundefs = 0; 
    printNames[i] = (char*)"loop"; i++;
    printNames[i] = (char*)"if"; i++
    printNames[i] = (char*)"block"; i++
    printNames[i] = (char*)"set"; i++
    printNames[i] = (char*)"+"; i++
    printNames[i] = (char*)"-"; i++
    printNames[i] = (char*)"*"; i++
    printNames[i] = (char*)"/"; i++
    printNames[i] = (char*)"="; i++
    printNames[i] = (char*)"<"; i++
    printNames[i] = (char*)">"; i++
    printNames[i] = (char*)"print"; i++

    numNames=i;
    numBuiltIns = 1; 
}; 

void stripComments(char input[], int length){ 
    // changes all items with a comment on into whitespace 
    // ex input: ( + x ; y )
    // ex output: ( + x space space space 
    for (int i = 0; i < length; i++) {
        // looping over the input array
        bool seenComment = false;
        if (input[i] == ';'){
            seenComment = true; 
        }
        if (seenComment == true) {
            // if we have already seen a comment, then
            // we need to discard the rest of the array---
            // e.g., we should erase them. 
            // our placeholder for deleting the item will be the space.
            input[i] = ' ';
        }
    }
}; 

bool areParenthesesBalanced(char input[], int length) 
{
    stack<char> s;
    for (int i = 0; i < length; i++) {
        if (input[i] == '(') {
            s.push(input[i]);
            continue;
        }
        else if (input[i] == ')'){ 
            if (s.empty()) { // checking to make sure we're not popping from an empty stack
                return false;
            }
            s.pop();
        }    
    }
    return s.empty(); 
};
int processInput(char input[], int length) {
    // returns a 1 if parenthesis match 
    // returns a 0 if parenthesis don't match
    stripComments(input, length); 
    


};

int main() {
    initNames(); 
    globalEnv = emptyEnv(); 
    char input[MAXINPUT];
    quittingtime = 0;
    while !(quittingtime){ 
        std::cout << PROMPT;
        std::cin.getline(input,MAXINPUT); // as a reminder, this only stores one line, and it doesn't include the new line (stops @ new line)
        processInput(input, (sizeof(input) / sizeof(input[0]))); 


        



