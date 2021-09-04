#include<iostream>
#include<fstream>
#include"../inc/assembler.h"

using namespace std;

int main(int argc, char *argv[]){
    string input = argv[1];
    string o = argv[2];
    string output;

    if(o == "-o"){
        output = argv[3];
    }

    //string inputString = "../tests/test1.txt";
    //string outputString = "../tests/output.txt";

    Assembler assembler = Assembler(input, output);
    
    
}