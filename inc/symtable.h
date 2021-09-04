#include<iostream>

using namespace std;

static int posId = 0;

class SymTabEntry {

    string name;
    string section;
    int value;
    char visibility;
    int index;
    bool isExtern = false;

public:

    SymTabEntry(string name, string section, int value){
        this->name = name;
        this->section = section;
        this->value = value;
        this->index = ++posId;  
    }

    SymTabEntry() {}

    SymTabEntry(string name){
        this->name = name;
        this->section = "UND";
        this->value = -9999;
        this->index = ++posId;
    }

    SymTabEntry(string name, string section){
        this->name = name;
        this->section = section;
        if(name == "UND") {
            this->index = 0;
            this->value = 0;
            this->visibility = 'l';
            return;
        }
        this->value = -9999;
        this->index = ++posId;  
    }
    
    void setExtern(){
        this->isExtern = true;
    }

    int getIsExtern(){
        return isExtern;
    }

    int getValue() {
        return this->value;
    }

    int getIndex() {
        return this->index;
    }

    string getName(){
        return this->name;
    }

    void setVisibility(char v){
        this->visibility = v;
    }

    char getVisibility(){
        return this->visibility;
    }

    void setSection(string section){
        this->section = section;
    }

    string getSection() {
        return this->section;
    }

    void setValue(int _value){
        this->value = _value;
    }

};
