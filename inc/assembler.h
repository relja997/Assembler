#include<iostream>
#include<string>
#include<list>
#include "symtable.h"
#include "section.h"

using namespace std;

class Assembler{
private:
    int locationCounter = 0;

    Section* currentSection = nullptr;
    // list of parsed assembler lines
    list<string> lista;

    // symbol table list
    list<SymTabEntry> symbolTable;

    // sections list
    list<Section> sections;
public:
    // helper functions
    void readFile(string inputString);
    void trimLine(string &line);
    void resolve(string str);
    void processSection(string sec);
    bool isDirective(string str);
    void processDirective(string dir, string line);
    bool isDefined(string s);
    void processGlobal(string line, string dir);
    void processWord(string line);
    void processEqu(string line);
    bool isSectionDefined(string sec);
    void processLabel(string lab);
    bool isMnemonic(string s);
    void processMnemonic(string mnem, string line);

    // if label isn't tied to any memory location
    // you should show an error
    bool labelFound = false;

    // first pass and creating symbol table
    void firstPass();

    // second pass and code generation
    void secondPass();

    // helper functions for second pass
    void secondResolve(string str);
    void processWord_2(string str);
    void processOperandsWord(string ident);
    void listAppend();
    void processMnemonic_2(string mnem, string line);
    string processNumber(string num, bool pcrel = false);
    SymTabEntry getSymbol(string simb);
    int getSectionValue(string sec);
    void relocationAppend();
    void addToRelTable(string line, bool isPcRel = false);

    Assembler(string inputString, string outputString);


};

