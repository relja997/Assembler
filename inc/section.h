#include<iostream>
#include"../inc/relocation.h"

using namespace std;

class Section {
    string name;
    int length;

    public: 
    list<string> sectionCode;
    list<Relocation> relocationTable;

    Section(){};

    Section(string name_){
        name = name_;
        length = 0;
    }

    void setLength(int _length){
        length = _length;
    }

    int getLength() {
        return this->length;
    }

    string getName(){
        return name;
    }
};