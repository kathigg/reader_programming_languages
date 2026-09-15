// for the READER project, programming languages 
#include <iostream>
#include <stack>
#include <stdio.h>
#include <string.h>
#include <cstring> // need it for memset
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

// struct EXPLISTREC;
// typedef EXPLISTREC* EXPLIST;
// enum EXPTYPE { VALEXP, VAREXP, APEXP};
// enum EXPTYPE { VALEXP, VAREXP, APEXP };
// struct EXPREC {
//     EXPTYPE etype; // what type of expression
//     NUMBER num;
//     NAME variable; 
// };
// struct EXPLISTREC {
//     EXP head; 
//     EXPLIST tail;
// };
// struct VALUELISTREC {
//     NUMBER head; 
//     VALUELISTREC* tail;
//     typedef VALUELISTREC* VALUELIST;
// };
// struct NAMELISTREC {
//     NAME head; 
//     NAMELISTREC* tail;
//     typedef NAMELISTREC* NAMELIST;
// };
// struct ENVREC {
//     NAMELIST vars;
//     VALUELIST values;
// };
// typedef ENVREC* ENV; 
// struct FUNDEFREC {
//     NAME funname; 
//     NAMELIST formals;
//     EXP body; 
//     FUNDREC* nextfundef; 
// }; 
// typedef FUNDEFREC* FUNDEF; 
// FUNDEF fundefs; 
// ENV globalEnv; 
// EXP currentExp; 

// // ...

// EXP mkVALEXP(NUMBER n) {
//     EXP e;
//     e = new EXPREC;
//     e -> etype = VALEXP;
//     e -> num = n; 
//     return e; 
// } 

// // ...

// int lengthVL(VALUELIST vl){
//     int i = 0;
//     while (vl != 0){
//         i++; 
//         vl = vl -> tail;
//     }
//     return i; 
// }

// int lengthML(NAME fname) {
//     FUNDEF f;
//     f = fundefs; 
// } 

// // void newFunDef(NAME fname; NAMELIST nl; EXP e);

// // init names -- place all pre-defined names ino printNames 
// void initNames() {
//     int i = 0; 
//     fundefs = 0; 
//     printNames[i] = (char*)"loop"; i++;
//     printNames[i] = (char*)"if"; i++
//     printNames[i] = (char*)"block"; i++
//     printNames[i] = (char*)"set"; i++
//     printNames[i] = (char*)"+"; i++
//     printNames[i] = (char*)"-"; i++
//     printNames[i] = (char*)"*"; i++
//     printNames[i] = (char*)"/"; i++
//     printNames[i] = (char*)"="; i++
//     printNames[i] = (char*)"<"; i++
//     printNames[i] = (char*)">"; i++
//     printNames[i] = (char*)"print"; i++

//     numNames=i;
//     numBuiltIns = 1; 
// }; 

void stripComments(char input[], int length){ 
    // changes all items with a comment on into whitespace 
    // ex input: ( + x ; y )
    // ex output: ( + x space space space 
    for (int i = 0; i < length; i++) {
        // looping over the input array
        bool seenComment = false;
        if (input[i] == ';'){
            seenComment = true; 
            input[i] = '#';
            continue; 
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

void issuePrompt(char input[], int length) {
    stripComments(input, length); // remove comments from the input
    bool matchingParens = areParenthesesBalanced(input, length); // stores whether parentheses match or not
    if (matchingParens) {
        // if parentheses match, issue prompt 1
        std::cout << PROMPT;
    }
    else if (!matchingParens) {
        // if parentheses don't match, issue prompt 2
        std::cout << PROMPT2;
    }
};

int main() {
    // initNames(); 
    // globalEnv = emptyEnv(); 
    char input[MAXINPUT];
    char newLine[MAXINPUT/2]; 
    bool quittingtime = 0;
    std::cout << PROMPT;
    while (!quittingtime){ 
        std::cin.getline(newLine,MAXINPUT); // as a reminder, this only stores one line, and it doesn't include the new line (stops @ new line)
        int newLineLength = strlen(newLine); 
        int length = strlen(input); 

        // if the input says quit, then exit and terminate the program
        if (strcmp(input, "quit") == 0) {
            quittingtime = 1; 
            break; 
        }

        // is the input string empty?
        if (length == 0) {
            strcpy(input, newLine); 
        }
        // is the input string not empty? 
        else if (length > 0) {
            if (length + newLineLength + 2 < MAXINPUT) {
                // concatenate the old string and new string 
                strcat(input, " "); 
                strcat(input, newLine); 
            }
            else {
                std::cout << "Buffer full!";
                std::cout << length; 
                quittingtime = 1; 
                break;
        }

        // if parentheses are balanced for the line, then issue the correct prompt and then clear the array
        if (areParenthesesBalanced(input, length)) {
            // issue the correct prompt
            issuePrompt(input, length);
            // clear the array
            input[0] = '\0'; 
            continue; 
        }
        // if parentheses are not balanced for the line, we won't overwrite the array
        else if (!areParenthesesBalanced(input, length)) {
            // issue the correct prompt
            issuePrompt(input, length);
            continue; 
        }
    }
    } 





    } 
        
        
        


        



