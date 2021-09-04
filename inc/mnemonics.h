#include<iostream>
#include<map>

using namespace std;

class Mnemonics {
public:
    map<string,string> mnemonics;

    Mnemonics() {
        mnemonics.insert({"halt", "0x00"});
        mnemonics.insert({"int", "0x10"});
        mnemonics.insert({"iret", "0x20"});
        mnemonics.insert({"call", "0x30"});
        mnemonics.insert({"ret", "0x40"});
        mnemonics.insert({"jmp", "0x50"});
        mnemonics.insert({"jeq", "0x51"});
        mnemonics.insert({"jne", "0x52"});
        mnemonics.insert({"jgt", "0x53"});
        mnemonics.insert({"xchg", "0x60"});
        mnemonics.insert({"add", "0x70"});
        mnemonics.insert({"sub", "0x71"});
        mnemonics.insert({"mul", "0x72"});
        mnemonics.insert({"div", "0x73"});
        mnemonics.insert({"cmp", "0x74"});
        mnemonics.insert({"not", "0x80"});
        mnemonics.insert({"and", "0x81"});
        mnemonics.insert({"or", "0x82"});
        mnemonics.insert({"xor", "0x83"});
        mnemonics.insert({"test", "0x84"});
        mnemonics.insert({"shl", "0x90"});
        mnemonics.insert({"shr", "0x91"});
        mnemonics.insert({"ldr", "0xA0"});
        mnemonics.insert({"str", "0xB0"});
    };

};