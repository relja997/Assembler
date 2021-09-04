#include "../inc/assembler.h"
#include<fstream>
#include<cctype>
#include<iomanip>
#include "../inc/mnemonics.h"
#include<regex>


void Assembler::trimLine(string &line){
    int startIndex = 0;
    int endIndex = line.size() - 1;
    for(int i = 0; i < line.size(); i++){
        if(!isblank(line[i])){
            startIndex = i;
            break;
        }
    }

    for(int i = line.size()-1; i >=0; i--) {
        if(!isblank(line[i])){
            endIndex = i;
            break;
        }
    }

    int comm = line.find_first_of("#");
        if(comm != string::npos)
            endIndex = comm;

    line = line.substr(startIndex, endIndex - startIndex + 1);
}

void Assembler::readFile(string inputString) {
    ifstream rdFile;
    string newLine;

    rdFile.open(inputString);

    if(rdFile.is_open()){
        while(getline(rdFile, newLine)){
            trimLine(newLine);
            // do not parse commented lines 
            // skip empty lines
            if(newLine[0] != '#' && !isblank(newLine[0]) && newLine.size() != 0)
                lista.push_back(newLine);
        }
    } else {
        cout<<"Greska pri otvaranju fajla"<<endl;
    }

    rdFile.close();

}

void Assembler::processSection(string sec){
    if(sec == ".text"){
        Section newSection(".text");
        sections.push_back(newSection);
    } else if(sec == ".data"){
        Section newSection(".data");
        sections.push_back(newSection);
    } else if(sec == ".bss"){
        Section newSection(".bss");
        sections.push_back(newSection);
    } else {
        Section newSection(sec);
        sections.push_back(newSection);
    }

    if(currentSection->getName() != "UND"){
        list<Section>::iterator it;
        for( it = sections.begin(); it != sections.end(); it++){
            if(it->getName() == currentSection->getName()){
                it->setLength(locationCounter);
                break;
            }
        }
    }
    currentSection = new Section(sec);
    locationCounter = 0;
}

bool Assembler::isDirective(string str){
    if(str == ".global" || str == ".extern" || str == ".word" || str == ".skip" || str == ".equ")
        return true;

    return false;
}

bool Assembler::isDefined(string s){
    list<SymTabEntry> :: iterator it;
    for(it = symbolTable.begin(); it != symbolTable.end(); it++){
        if(it->getName() == s)
            return true;
    }
    return false;
}

void Assembler::processGlobal(string line, string dir){
    for(int i = 0; i < line.size(); i++){
            string ident = "";
            if(isalpha(line[i])){
                while(isalnum(line[i])){
                    ident += line[i];
                    i++;
                }
                if(!isDefined(ident)){
                    SymTabEntry* newSymbol = new SymTabEntry(ident);
                    newSymbol->setVisibility('g');
                    if(dir == ".extern")
                        newSymbol->setExtern();
                    symbolTable.push_back(*newSymbol);
                } else {
                    cout<<"Simbol "<<ident<<" je vec definisan"<<endl;
                    return;
                }
            }
        }
}

void Assembler::processWord(string line){
    int numOfLiterals = 0;
    for(int i = 0; i < line.size(); i++){
            string ident = "";
            if(isalnum(line[i])){
                while(isalnum(line[i])){
                    ident += line[i];
                    i++;
                }
                numOfLiterals++;
            }
    }
    locationCounter += numOfLiterals * 2;
}

void Assembler::processEqu(string line){
    int i = 0;
    string ident = "";
    while(isalpha(line[i]) || line[i] == '_'){
        ident += line[i];
        i++;
    }

    // skip comma and space
    i += 2;

    line = line.substr(i);
    int value = stoi(line, nullptr, 0);
    
    bool isDefined = false;
    for(list<SymTabEntry>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++){
        if(it->getName() == ident){
            isDefined = true;
            it->setValue(value);
            it->setSection("-");
        }
    }
    if(!isDefined){
        SymTabEntry* newSymbol = new SymTabEntry(ident, "-", value);
        newSymbol->setVisibility('l');
        symbolTable.push_back(*newSymbol);
    }
}

void Assembler::processDirective(string dir, string line){
    if(line[line.size() - 1] == '#'){
        line = line.substr(0, line.size() - 1);
    }
    if(dir == ".global" || dir == ".extern"){
        processGlobal(line, dir);
    }

    if(dir == ".skip"){
        if(currentSection->getName() == "UND"){
            cout<<".skip instrukcija se koristi van sekcije!"<<endl;
            exit(1);
        }
        locationCounter += stoi(line, nullptr, 0);
    }

    if(dir == ".word"){
        if(currentSection->getName() == "UND"){
            cout<<".word instrukcija se koristi van sekcije!"<<endl;
            exit(1);
        }
        processWord(line);
    }

    if(dir == ".equ"){
        processEqu(line);
    }
}

bool Assembler::isSectionDefined(string sec){
    list<Section> :: iterator it;
    for(it = sections.begin(); it != sections.end(); it++){
        if(it->getName() == sec)
            return true;
    }
    return false;
}

void Assembler::processLabel(string lab){
    string ident = lab.substr(0, lab.size() - 1);

    list<SymTabEntry> :: iterator it;
    for(it = symbolTable.begin(); it != symbolTable.end(); it++){
        if(it->getName() == ident){
            if(it->getIsExtern()){
                cout<<"Ne moze se definisati eksterni simbol! "<<endl;
                exit(1);
            }
            it->setValue(locationCounter);
            it->setSection(currentSection->getName());
            return;
        }
    }
    SymTabEntry* newSymbol = new SymTabEntry(ident, currentSection->getName(), locationCounter);
    newSymbol->setVisibility('l');
    symbolTable.push_back(*newSymbol);
}

bool Assembler::isMnemonic(string s){
    Mnemonics mn = Mnemonics();
    if(mn.mnemonics.find(s) != mn.mnemonics.end())
        return true;
    if(s == "push" || s == "pop")
        return true;
    return false;
}

void Assembler::processMnemonic(string mnem, string line){
    // if mnemonic has no operands
    if(line == ""){
        locationCounter++;
    }

    if(mnem == "int" || mnem == "not"){
        locationCounter += 2;
    }

    // push and pop
    // interpreted as str, ldr respectively with register indirect addressing
    if(mnem == "push" || mnem == "pop"){
        locationCounter += 3;
    }

    // arithmetical-logical operations
    if(mnem == "xchg" || mnem == "add" || mnem == "sub" || mnem == "mul"
     || mnem == "div" || mnem == "cmp" || mnem == "and" || mnem == "or"
     || mnem == "xor" || mnem == "test" || mnem == "shl" || mnem == "shr") {

            locationCounter += 2;
    }

    // jumps
    if(mnem == "jmp" || mnem == "jeq" || mnem == "jne" || mnem == "jgt" || mnem == "call"){
        locationCounter += 3;
        // bez zvjezdice
        if(isalnum(line[0]) || line[0] == '%' || line[0] == '-'){
            locationCounter += 2;
        }
        if(line[0] == '*'){
            regex r("^(r[0-7])$");
            regex r1("\\+");
            // ukoliko nije memorijsko
            if(isalnum(line[1])){
                int i = 1;
                string operand = "";
                while(isalnum(line[i])){
                    operand += line[i];
                    i++;
                }
                if(!regex_search(operand, r)){
                    locationCounter += 2;
                }
            }
            // ukoliko je memorijsko
            else if(line[1] == '['){
                int i = 1;
                string operand = "";
                while(line[i] != ']'){
                    operand += line[i];
                    i++;
                }
                operand += ']';

                if(regex_search(operand, r1)){
                    locationCounter += 2;
                }
            }
            
        }
    }

    // ldr and str instructions
    if(mnem == "ldr" || mnem == "str" ){
        locationCounter += 3;
        int i = 0;
        while(line[i] != ','){
            i++;
        }
        // skip blank space
        i += 2;
        int t = i;
        if(line[i] == '$'){
            locationCounter += 2;
        }
        if(isalnum(line[i])){
            regex r("^(r[0-7])$");
            string operand = "";
            while(isalnum(line[i])){
                operand += line[i];
                i++;
            }
            if(!regex_search(operand, r)){
                locationCounter += 2;
            }
        }
        if(line[t] == '['){
                int i = t;
                regex r1("\\+");
                string operand = "";
                while(line[i] != ']'){
                    operand += line[i];
                    i++;
                }
                operand += ']';

                if(regex_search(operand, r1)){
                    locationCounter += 2;
                }
        }
        if(line[t] == '%'){
            locationCounter += 2;
        }

    }
}

void Assembler::resolve(string str){
    if(str[str.size()-1] == ':'){
        processLabel(str);
        labelFound = true;
        return;
    }
    if(str.find(".end") != string::npos){
        if(currentSection->getName() != "UND"){
        list<Section>::iterator it;
        for( it = sections.begin(); it != sections.end(); it++){
            if(it->getName() == currentSection->getName()){
                it->setLength(locationCounter);
                break;
            }
        }
    }
    }
    // take the first word from line in temporary string
    string temp1 = "";
    string temp2 = "";
    int i = 0;
    while(!isblank(str[i])){
        temp1 += str[i];
        i++;
    }
    // skip the next blank character
    while(isblank(str[i]) && i < str.size()){
        i++;
    }

    // check what is the first word

    if(temp1[temp1.size()-1] == ':'){
        processLabel(temp1);
        labelFound = true;
        if(!isblank(str[i]) && str[i] != '#'){
            labelFound = false;
            str = str.substr(i, str.size() - i);
            resolve(str);
            return;
        }
    }

    // if a directive is .section add it to section list to link it to symbolTable afterwards
    if(temp1 == ".section"){
        if(labelFound == true) {
            cout<<"Definisana labela nije vezana za memorijsku lokaciju!"<<endl;
            exit(1);
        }
        // slucaj sa komentarom u definiciji sekcije
        int comm = str.find_first_of("#");
        if(comm != string::npos){
            temp2 = str.substr(i, comm - i - 1);
        } else
            temp2 = str.substr(i, str.size() - i);
        if(isSectionDefined(temp2)){
            cout<<"Sekcija "<<temp2<<" je vec prethodno definisana!"<<endl;
            exit(1);
        }
        processSection(temp2);
    }

    
    // sections without .section directive
    if(temp1.find(".text") != string::npos) {
        processSection(".text");
    }
    
    if(temp1.find(".data") != string::npos) {
        processSection(".data");
    }

    if(temp1.find(".bss") != string::npos) {
        processSection(".bss");
    }

    labelFound = false;
    // if the line contains a directive
    if(isDirective(temp1)){
        processDirective(temp1, str.substr(i, str.size() - 1));
        return;
    }
    // if the line contains a mnemonic
    if(isMnemonic(str)){
        processMnemonic(str, "");
    }
    if(isMnemonic(temp1)){
        processMnemonic(temp1, str.substr(i));
    }
    //cout<<str<<'\t'<<locationCounter<<endl;
}

void Assembler::firstPass() {
    SymTabEntry* und = new SymTabEntry("UND", "-");
    symbolTable.push_back(*und);
    currentSection = new Section("UND");
    list<string> :: iterator it;

    for(it = lista.begin(); it != lista.end(); it++){
        resolve(*it);
    }
    list<Section> :: iterator itera;
    for(itera = sections.begin(); itera != sections.end(); itera++){
        if(itera->getName() != ".bss"){
            SymTabEntry* newSymbol = new SymTabEntry(itera->getName(), itera->getName(), itera->getLength());
            newSymbol->setVisibility('l');
            symbolTable.push_front(*newSymbol);
        }
    }

    list<SymTabEntry> :: iterator iter;
    for(iter = symbolTable.begin(); iter != symbolTable.end(); iter++){
        if(iter->getSection() == "UND" && !iter->getIsExtern()){
            cout<<"Simbol "<<iter->getName()<<" je nedefinisan!"<<endl;
            exit(1);
        }
    }

}

// ================================= SECOND PASS =================================

void Assembler::processOperandsWord(string ident){
            char visibility = 'l';            
            // if the ident is a symbol
            if(isalpha(ident[0])){
                list<SymTabEntry> :: iterator it;
                for(it = symbolTable.begin(); it != symbolTable.end(); it++){
                    if(it->getName() == ident){
                        ident = to_string(it->getValue());
                        visibility = it->getVisibility();
                    }
                }
            }
            
            if(visibility == 'g'){
                currentSection->sectionCode.push_back("0x00");
                currentSection->sectionCode.push_back("0x00");
                return;
            }

            if(ident[0] == '0' && ident.size() == 1){
                currentSection->sectionCode.push_back("0x00");
                currentSection->sectionCode.push_back("0x00");
            }
            
            // if a literal is a hex number
            if(ident[0] == '0' && (ident[1] == 'x' || ident[1] == 'X')){
                currentSection->sectionCode.push_back(ident);
                currentSection->sectionCode.push_back("0x00");
            }
            // if a literal is a decimal number
            if(ident[0] != '0' && !isalpha(ident[0])){
                std::stringstream ss;
                ss<< std::hex << stoi(ident); // int decimal_value
                if(stoi(ident) <= 255 && stoi(ident) >= -256){
                    std::string res ( ss.str() );
                    if(stoi(ident) >= 0){
                        string x = "0x";
                        if(stoi(ident) < 16)
                            x += "0";
                        x += res;
                        currentSection->sectionCode.push_back(x);
                        currentSection->sectionCode.push_back("0x00");
                    } else {
                        string x = "0x";
                        x += res[res.size() - 2];
                        x += res[res.size() - 1];
                        currentSection->sectionCode.push_back(x);
                        currentSection->sectionCode.push_back("0xff");
                    }
                }
                else {
                    std::stringstream ss;
                    ss<< std::hex << stoi(ident); // int decimal_value
                    std::string res ( ss.str() );
                    if(stoi(ident) < 4096 && stoi(ident) >= -4096){
                        if(stoi(ident) >= 0){
                            string x = "0x0";
                            x += res[0];
                            string byte_2 = "0x";
                            byte_2 += res[res.size()-2];
                            byte_2 += res[res.size() - 1];
                            currentSection->sectionCode.push_back(byte_2);
                            currentSection->sectionCode.push_back(x);
                        } else {
                            string x = "0x";
                            x += res[res.size() - 2];
                            x += res[res.size() - 1];
                            string y = "0x";
                            y += res[res.size() - 4];
                            y += res[res.size() - 3];
                            currentSection->sectionCode.push_back(x);
                            currentSection->sectionCode.push_back(y);
                        }
                    } else {
                        std::string res ( ss.str() );
                        string x = "0x";
                        x += res[res.size() - 2];
                        x += res[res.size() - 1];
                        string y = "0x";
                        y += res[res.size() - 4];
                        y += res[res.size() - 3];
                        currentSection->sectionCode.push_back(x);
                        currentSection->sectionCode.push_back(y);
                    }
                }
            }
}

SymTabEntry Assembler::getSymbol(string ident){
    list<SymTabEntry> :: iterator it;
    for(it = symbolTable.begin(); it != symbolTable.end(); it++){
        if(it->getName() == ident){
            return *it;
        }
    }
    return SymTabEntry();
}

int Assembler::getSectionValue(string sec){
    list<SymTabEntry> :: iterator it;
    for(it = symbolTable.begin(); it != symbolTable.end(); it++){
        if(it->getName() == sec){
            return it->getIndex();
        }
    }
    return -1;
}

void Assembler::processWord_2(string str){
    for(int i = 0; i < str.size(); i++){
            string ident = "";
            if(isblank(str[i])) i++;
            while(str[i] != ',' && i < str.size()){
                ident += str[i];
                i++;
            }
            if(isalpha(ident[0])){
                string offset = processNumber(to_string(locationCounter));
                Relocation* newRel = new Relocation(offset, "R_386_32");
                SymTabEntry symbol = getSymbol(ident);
                if(symbol.getVisibility() == 'g'){
                    newRel->setValue(symbol.getIndex());
                } else {
                    string section = symbol.getSection();
                    int value = getSectionValue(section);
                    newRel->setValue(value);
                }
                if(symbol.getSection() != "-"){
                    currentSection->relocationTable.push_back(*newRel);
                }
            }

            processOperandsWord(ident);
            locationCounter += 2;
            relocationAppend();
        }
            /*
            list<Relocation>::iterator it;
            cout<<currentSection->getName()<<endl;
            for(it = currentSection->relocationTable.begin(); it != currentSection->relocationTable.end(); it++){
                cout<<it->getOffset()<<'\t'<<it->getRelType()<<'\t'<<it->getValue()<<endl;
            }
            */
           
            listAppend();
}

void Assembler::relocationAppend(){
    list<Section>::iterator it;
    for(it = sections.begin(); it != sections.end(); it++){
        if(it->getName() == currentSection->getName())
            //cout<<it->getName()<<endl;
            it->relocationTable.splice(it->relocationTable.end(), currentSection->relocationTable);
    }
}

void Assembler::listAppend(){
    list<Section>::iterator it;
    for(it = sections.begin(); it != sections.end(); it++){
        if(it->getName() == currentSection->getName())
            //cout<<it->getName()<<endl;
            it->sectionCode.splice(it->sectionCode.end(), currentSection->sectionCode);
    }
}

string Assembler::processNumber(string ident, bool pcrel){
            char visibility = 'l';
             // if the ident is a symbol
            if(isalpha(ident[0])){
                list<SymTabEntry> :: iterator it;
                for(it = symbolTable.begin(); it != symbolTable.end(); it++){
                    if(it->getName() == ident){
                        int val = it->getValue();
                        if(pcrel) val -= 2;
                        ident = to_string(val);
                        visibility = it->getVisibility();
                    }
                }
            }
            
            if(visibility == 'g'){
                if(pcrel){
                    return "0xfe 0xff";
                }
                return "0x00 0x00";
            }
            // if a literal is a hex number
            if(ident[0] == '0' && (ident[1] == 'x' || ident[1] == 'X')){
                if(ident.size() == 3){
                    string rez = "0x0";
                    rez += ident[2];
                    rez += " 0x00";
                    return rez;
                }
                if(ident.size() == 4){
                    string rez = ident + " 0x00";
                    return rez;
                }
                if(ident.size() == 6){
                    string rez = "0x";
                    rez += ident.substr(4, 2);
                    rez += " 0x";
                    rez += ident.substr(2, 2);
                    return rez;
                }
            }

            if(ident[0] == '0' && ident.size() == 1){
                return "0x00 0x00";
            }
            // if a literal is a decimal number
            if(ident[0] != '0' && !isalpha(ident[0])){
                std::stringstream ss;
                ss<< std::hex << stoi(ident); // int decimal_value
                if(stoi(ident) <= 255 && stoi(ident) >= -256){
                    std::string res ( ss.str() );
                    if(stoi(ident) >= 0){
                        string x = "0x";
                        if(stoi(ident) < 16)
                            x += "0";
                        x += res;
                        x += " 0x00";
                        return x;
                    } else {
                        string x = "0x";
                        x += res[res.size() - 2];
                        x += res[res.size() - 1];
                        x += " 0xff";
                        return x;
                    }
                }
                else {
                    std::stringstream ss;
                    ss<< std::hex << stoi(ident); // int decimal_value
                    std::string res ( ss.str() );
                    if(stoi(ident) < 4096 && stoi(ident) >= -4096){
                        if(stoi(ident) >= 0){
                            string x = " 0x0";
                            x += res[0];
                            string byte_2 = "0x";
                            byte_2 += res[res.size()-2];
                            byte_2 += res[res.size() - 1];
                            byte_2 += x;
                            return byte_2;
                        } else {
                            string x = "0x";
                            x += res[res.size() - 2];
                            x += res[res.size() - 1];
                            string y = " 0x";
                            y += res[res.size() - 4];
                            y += res[res.size() - 3];
                            x += y;
                            return x;
                        }
                    } else {
                        std::string res ( ss.str() );
                        string x = "0x";
                        x += res[res.size() - 2];
                        x += res[res.size() - 1];
                        string y = " 0x";
                        y += res[res.size() - 4];
                        y += res[res.size() - 3];
                        x += y;
                        return x;
                    }
                }
            }
            return "";
}

void Assembler::addToRelTable(string line, bool isPcRel){
    string offset = processNumber(to_string(locationCounter - 2));
    Relocation* newRel = new Relocation(offset, "R_386_32");
    if(isPcRel){
        newRel->setRelType("R_386_PC32");
    }
    SymTabEntry symbol = getSymbol(line);
    // if it is an absolute symbol, do not create relocation record
    if(symbol.getSection() == "-")
        return;
    // if it is a global symbol put its index from symbol table to relocation record
    if(symbol.getVisibility() == 'g'){
        newRel->setValue(symbol.getIndex());
    // if it is a local symbol put its section index to relocation record
    } else {
        string section = symbol.getSection();
        int value = getSectionValue(section);
        newRel->setValue(value);
    }
    currentSection->relocationTable.push_back(*newRel);
    //cout<<line<<'\t'<<newRel->getOffset()<<'\t'<<newRel->getRelType()<<'\t'<<newRel->getValue()<<endl;
}

void Assembler::processMnemonic_2(string mnem, string line){
    Mnemonics mn = Mnemonics();
    if(line == ""){
        locationCounter++;
        currentSection->sectionCode.push_back(mn.mnemonics[mnem]);
    }
    if(mnem == "int" || mnem == "not"){
        locationCounter += 2;
        int i = 0;
        while(line[i] != 'r')
            i++;
        i++;
        int reg = line[i] - '0';
        string code = "0x";
        code += to_string(reg);
        code += "0";
        currentSection->sectionCode.push_back(mn.mnemonics[mnem]);
        currentSection->sectionCode.push_back(code);
    }
    // arithmetical-logical operations
    if(mnem == "xchg" || mnem == "add" || mnem == "sub" || mnem == "mul"
     || mnem == "div" || mnem == "cmp" || mnem == "and" || mnem == "or"
     || mnem == "xor" || mnem == "test" || mnem == "shl" || mnem == "shr") {
         
            locationCounter += 2;

        int i = 0;
        while(line[i] != 'r')
            i++;
        i++;
        int regD = line[i] - '0';
        string code = "0x";
        code += to_string(regD);
        while(line[i] != 'r')
            i++;
        i++;
        int regS = line[i] - '0';
        code += to_string(regS);   
        currentSection->sectionCode.push_back(mn.mnemonics[mnem]);
        currentSection->sectionCode.push_back(code);
    }

    if(mnem == "push" || mnem == "pop"){
        locationCounter += 3;

        int i = 0;
        while(line[i] != 'r')
            i++;
        i++;
        int regD = line[i] - '0';
        if(mnem == "push"){
            string opCode = mn.mnemonics["str"];
            string dest = "0x";
            dest += to_string(regD);
            dest += "6";
            string addr = "0x12";
            currentSection->sectionCode.push_back(opCode);
            currentSection->sectionCode.push_back(dest);
            currentSection->sectionCode.push_back(addr);
        }
        if(mnem == "pop"){
            string opCode = mn.mnemonics["ldr"];
            string dest = "0x";
            dest += to_string(regD);
            dest += "6";
            string addr = "0x42";
            currentSection->sectionCode.push_back(opCode);
            currentSection->sectionCode.push_back(dest);
            currentSection->sectionCode.push_back(addr);
        }
    }

    // jumps
    if(mnem == "jmp" || mnem == "jeq" || mnem == "jne" || mnem == "jgt" || mnem == "call"){
        locationCounter += 3;
        // bez zvjezdice
        if(isalnum(line[0]) || line[0] == '%' || line[0] == '-'){
            locationCounter += 2;
            string opCode = mn.mnemonics[mnem];
            string registers = "0x00";
            if(isdigit(line[0]) || (line[0] == '0' && (line[1] == 'x') || (line[1] == 'X')) || line[0] == '-'){
                string addr = "0x00";
                int i = 0;
                string value = "";
                while(!isblank(line[i])){
                    i++;
                }
                value = line.substr(0, i);
                string payload = processNumber(value);
                currentSection->sectionCode.push_back(opCode);
                currentSection->sectionCode.push_back(registers);
                currentSection->sectionCode.push_back(addr);
                currentSection->sectionCode.push_back(payload);
            }

            if(isalpha(line[0])){
                //cout<<mnem<<' '<<line<<'\t'<<locationCounter<<endl;
                addToRelTable(line);
                int i = 0;
                string addr = "0x00";
                string value = "";
                while(!isblank(line[i])){
                    i++;
                }
                value = line.substr(0, i);
                string payload = processNumber(value);
                currentSection->sectionCode.push_back(opCode);
                currentSection->sectionCode.push_back(registers);
                currentSection->sectionCode.push_back(addr);
                currentSection->sectionCode.push_back(payload);
                //cout<<mnem<<' '<<line<<'\t'<<mn.mnemonics[mnem]<<' '<<registers<<' '<<addr<<' '<<payload<<endl;
                
            }
            if(line[0] == '%'){
                string registers = "0x70";
                string addr = "0x05";
                int i = 1;
                while(isalpha(line[i]) && i < line.size()){
                    i++;
                }
                string operand = line.substr(1, i - 1);
                addToRelTable(operand, true);
                string payload = processNumber(operand, true);
                
                currentSection->sectionCode.push_back(opCode);
                currentSection->sectionCode.push_back(registers);
                currentSection->sectionCode.push_back(addr);
                currentSection->sectionCode.push_back(payload);

            }
        }
        if(line[0] == '*'){
            regex r("^(r[0-7])$");
            regex r1("\\+");
            // ukoliko nije memorijsko
            if(isalnum(line[1])){
                int i = 1;
                string operand = "";
                while(isalnum(line[i])){
                    operand += line[i];
                    i++;
                }
                if(!regex_search(operand, r)){
                    locationCounter += 2;
                    if(isalpha(operand[0])){
                        addToRelTable(operand);
                    }
                    string opCode = mn.mnemonics[mnem];
                    string registers = "0x00";
                    string addr = "0x04";
                    string payload = processNumber(operand);    
                    currentSection->sectionCode.push_back(opCode);
                    currentSection->sectionCode.push_back(registers);
                    currentSection->sectionCode.push_back(addr);
                    currentSection->sectionCode.push_back(payload);
                } else {
                    int i = 0;
                    while(!isdigit(line[i])){
                        i++;
                    }
                    int value = line[i] - '0';
                    string opCode = mn.mnemonics[mnem];
                    string addr = "0x01";
                    string registers = "0x";
                    registers += to_string(value);
                    registers += "0";
                    currentSection->sectionCode.push_back(opCode);
                    currentSection->sectionCode.push_back(registers);
                    currentSection->sectionCode.push_back(addr);
                }
            }
            // ukoliko je memorijsko
            else if(line[1] == '['){
                int i = 2;
                int t = line.size() - 1;
                string operand = "";
                if(!isalpha(line[i])){
                    while(!isalnum(line[i])){
                        i++;
                    }
                }
                if(!isalpha(line[t])){
                    while(!isalnum(line[t])){
                        t--;
                    }
                }
                string opCode = mn.mnemonics[mnem];
                operand = line.substr(i, t - i + 1);
                int reg = operand[1] - '0';
                string registers = "0x";
                registers += to_string(reg);
                registers += "0";

                if(regex_search(operand, r1)){
                    string addr = "0x03";
                    locationCounter += 2;
                    int i = 2;
                    string minus = "";
                    while(!isalnum(operand[i])){
                        i++;
                        if(operand[i] == '-')
                            minus = "-";
                    }
                    int t = i;
                    while(isalnum(operand[i])){
                        i++;
                    }
                    operand = operand.substr(t, t - i);
                    if(isalpha(operand[0])){
                        addToRelTable(operand);
                    }
                    string sabirak = minus;
                    sabirak += operand;
                    string payload = processNumber(sabirak);
                    currentSection->sectionCode.push_back(opCode);
                    currentSection->sectionCode.push_back(registers);
                    currentSection->sectionCode.push_back(addr);
                    currentSection->sectionCode.push_back(payload);
                } else{
                    string addr = "0x02";
                    currentSection->sectionCode.push_back(opCode);
                    currentSection->sectionCode.push_back(registers);
                    currentSection->sectionCode.push_back(addr);
                }
            }
            
        }
    }

    // ldr and str instructions
    if(mnem == "ldr" || mnem == "str" ){
        locationCounter += 3;
        string opCode = mn.mnemonics[mnem];
        int i = 0;
        while(!isblank(line[i]) && line[i] != ','){
            i++;
        }
        string regD = line.substr(0, i);
        int reg = regD[i-1] - '0';
        regD = "0x" + to_string(reg);
        i++;
        while(isblank(line[i])){
            i++;
        }
        // skip blank space
        int t = i;
        if(line[i] == '$'){
            locationCounter += 2;
            string addr = "0x00";
            string registers = regD + "0";
            while(!isblank(line[i])){
                i++;
            }
            string operand = line.substr(t + 1, i - t);
            if(isalpha(operand[0])){
                addToRelTable(operand);
            }
            string payload = processNumber(operand);
            currentSection->sectionCode.push_back(opCode);
            currentSection->sectionCode.push_back(registers);
            currentSection->sectionCode.push_back(addr);
            currentSection->sectionCode.push_back(payload);
        }
        if(isalnum(line[i])){
            regex r("^(r[0-7])$");
            int t = i;
            while(isalnum(line[i]) || line[i] == '_'){
                i++;
            }
            string operand = line.substr(t, i - t);
            if(!regex_search(operand, r)){
                locationCounter += 2;
                if(isalpha(operand[0])){
                    addToRelTable(operand);
                }
                string addr = "0x04";
                string registers = regD + "0";
                string payload = processNumber(operand);
                //cout<<mnem<<' '<<line<<'\t'<<mn.mnemonics[mnem]<<' '<<registers<<' '<<addr<<' '<<payload<<endl;
                currentSection->sectionCode.push_back(opCode);
                currentSection->sectionCode.push_back(registers);
                currentSection->sectionCode.push_back(addr);
                currentSection->sectionCode.push_back(payload);
            } else {
                // operand je drugi registar
                int regS = operand[operand.size() - 1] - '0';
                string addr = "0x01";
                string registers = regD + to_string(regS);
                //cout<<mnem<<' '<<line<<'\t'<<opCode<<' '<<registers<<' '<<addr<<endl;
                currentSection->sectionCode.push_back(opCode);
                currentSection->sectionCode.push_back(registers);
                currentSection->sectionCode.push_back(addr);
            }
        }
        if(line[t] == '['){
                int i = t + 1;
                regex r1("\\+");
                string operand = "";
                while(line[i] != ']'){
                    operand += line[i];
                    i++;
                }

                if(regex_search(operand, r1)){
                    locationCounter += 2;
                    string addr = "0x03";
                    int regS = operand[1] - '0';
                    string registers = regD + to_string(regS);
                    int i = 2;
                    string minus = "";
                    while(!isalnum(operand[i])){
                        i++;
                        if(operand[i] == '-')
                            minus = "-";
                    }
                    int t = i;
                    while(!isblank(operand[i])){
                        i++;
                    }
                    string reloc = operand.substr(t, i-t);
                    if(isalpha(reloc[0])) {
                        addToRelTable(reloc);
                    }
                    string op = minus;
                    op += operand.substr(t, i - t);
                    string payload = processNumber(op);
                    currentSection->sectionCode.push_back(opCode);
                    currentSection->sectionCode.push_back(registers);
                    currentSection->sectionCode.push_back(addr);
                    currentSection->sectionCode.push_back(payload);
                } else {
                    // ako stoji samo registar u operandu
                    string addr = "0x02";
                    int regS = operand[1] - '0';
                    string registers = regD + to_string(regS);
                    currentSection->sectionCode.push_back(opCode);
                    currentSection->sectionCode.push_back(registers);
                    currentSection->sectionCode.push_back(addr);
                }
        }
        if(line[t] == '%'){
            locationCounter += 2;
            string addr = "0x03";
            int p = t + 1;
            while(!isblank(line[t]) && t < line.size()){
                t++;
            }
            string operand = line.substr(p, t - p);
            addToRelTable(operand, true);
            string payload = processNumber(operand, true);
            string registers = regD + "7";
            currentSection->sectionCode.push_back(opCode);
            currentSection->sectionCode.push_back(registers);
            currentSection->sectionCode.push_back(addr);
            currentSection->sectionCode.push_back(payload);
        }

    }

    listAppend();
    relocationAppend();
}

void Assembler::secondResolve(string str){
    string temp1 = "";
    string temp2 = "";
    int i = 0;
    while(!isblank(str[i]) && i < str.size()){
        i++;
    }
    temp1 = str.substr(0, i);

    if(temp1[temp1.size() - 1] == ':'){
        int comm = str.find_first_of("#");
        while(isblank(str[i])){
            i++;
        }
        if(comm != string::npos){
            if(i != comm){
                str = str.substr(i, comm - i - 1);
                secondResolve(str);
                return;
            }
        } else{
            str = str.substr(i, str.size() - i);
            if(isalpha(str[0])){
                secondResolve(str);
                return;
            }
        }
    }

    // skip the next blank character
    while(isblank(str[i]) && i < str.size()){
        i++;
    }
    
    // if a directive is a section

    if(temp1 == ".section"){
        // slucaj sa komentarom u definiciji sekcije
        int comm = str.find_first_of("#");
        if(comm != string::npos){
            temp2 = str.substr(i, comm - i - 1);
        } else
            temp2 = str.substr(i, str.size() - i);
        currentSection = new Section(temp2);
        locationCounter = 0;
    }

    if(temp1 == ".word"){
        int comm = str.find_first_of("#");
        if(comm != string::npos){
            temp2 = str.substr(i, comm - i);
        }
        else {
            temp2 = str.substr(i, str.size() - i);
        }
        processWord_2(temp2);
    }
    
    if(temp1 == ".skip"){
        int comm = str.find_first_of("#");
        if(comm != string::npos){
            temp2 = str.substr(i, comm - i);
        } else
            temp2 = str.substr(i, str.size() - i);
        
        int value = stoi(temp2, nullptr, 0);
        locationCounter += value;
        for(int i = 0; i < value; i++){
            currentSection->sectionCode.push_back("0x00");
        }

        listAppend();
    }
    if(isMnemonic(str)){
        processMnemonic_2(str, "");
        return;
    }
    if(isMnemonic(temp1)){
        processMnemonic_2(temp1, str.substr(i));
    }


}

void Assembler::secondPass(){
    locationCounter = 0;
    currentSection = new Section("UND");
    list<string> :: iterator it;
    for(it = lista.begin(); it != lista.end(); it++){
        secondResolve(*it);
    }
    
}

Assembler::Assembler(string inputString, string outputString){

    ofstream wrFile(outputString);

    readFile(inputString);

    /*
    list<string> :: iterator it;

    // write parsed .s code to output
    for(it = lista.begin(); it != lista.end(); it++){
        cout<<*it<<endl;
    }
    */
    
    firstPass();

    // print all the sections from the section list
    /*
    list<Section> :: iterator it;
    for(it = sections.begin(); it != sections.end(); it++){
        cout<<it->getName()<<endl;
    }*/


    // print symbolTable

    
    list<SymTabEntry> :: iterator iter;

    wrFile << left << setw(15) << "Name" << left << setw(15) << "Section" << left << setw(7) << "Value" << left << setw(15) << "Visibility" << left << setw(11) << "Index" << endl;

    for(iter = symbolTable.begin(); iter != symbolTable.end(); iter++){
        wrFile << left << setw(15) << iter->getName() << left << setw(15) << iter->getSection() << left << setw(7) << iter->getValue() << left << setw(15) << iter->getVisibility() << left << setw(11)<< iter->getIndex()<< endl;
    }
    
    wrFile<<endl;
    wrFile<<"###############################################################################"<<endl;
   secondPass();

   // print section content
    wrFile<<endl;
    list<Section> :: iterator itCon;
    for(itCon = sections.begin(); itCon != sections.end(); itCon++){
        wrFile<<itCon->getName()<<endl;
            list<string>::iterator iter;
            int i = 0;
            for(iter = itCon->sectionCode.begin(); iter != itCon->sectionCode.end(); iter++){
                wrFile<<*iter<<" ";
                if(i == 8){
                    wrFile<<endl;
                    i = 0;
                }
                i++;
            }
            wrFile<<endl;
    }
    
    wrFile<<"###############################################################################"<<endl;

   // print relocation table
    
    list<Section> :: iterator it;
    for(it = sections.begin(); it != sections.end(); it++){
        wrFile<<it->getName()<<endl;
        wrFile<<"#offset"<<"\t\t"<<"#type"<<"\t\t"<<"#value"<<endl;
            list<Relocation>::iterator iter;
            for(iter = it->relocationTable.begin(); iter != it->relocationTable.end(); iter++){
                wrFile<<iter->getOffset()<<'\t'<<iter->getRelType()<<'\t'<<iter->getValue()<<endl;
            }
            wrFile<<endl;
    }
    
    

    wrFile.close();
    
}