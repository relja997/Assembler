#include<iostream>

using namespace std;

class Relocation {
    string offset;
    string relType;
    int value;
public:
    Relocation(string _offset, string _relType, int _value){
        offset = _offset;
        relType = _relType;
        value = _value;
    }
    Relocation(string _offset, string _relType){
        offset = _offset;
        relType = _relType;
    }

    void setOffset(string _offset){
        offset = _offset;
    }
    
    void setRelType(string _relType){
        relType = _relType;
    }

    void setValue(int _value){
        value = _value;
    }

    string getOffset(){
        return offset;
    }

    string getRelType(){
        return relType;
    }

    int getValue(){
        return value;
    }

};