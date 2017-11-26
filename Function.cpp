//
// Created by Max on 2/7/2017.
//

#include <iostream>
#include <unordered_map>
#include "Function.h"

extern std::unordered_map<Function*, Function*> copy_map;

extern Stack *stack;

//Call the function pointer, then use its return value to determine what to run next
//Yes, it seems pointless, but it's needed due to the way inherited classes use the method.
int Function::run() {
    return fxn();
}

int Var::run() {
    stack->push(n);
    return 0;
}

int DoubleConst::run() {
    stack->push(n64);
    return 0;
}

//Prints the stored string. Simple enough.
int StrPrint::run() {
    printf("%s", str);
    return 0;
}

//Prints the stored string and aborts
int Abort::run() {
    printf("%s", str);
    abort_();
    return 0;
}

int UsrFunc::run() {
    Function* func = head;
    while(func->next) {
        int idx = func->run();
        func = func->next[idx];
    }
    return func->run();
}

//Number determination helper function
bool is_num(std::string& str) {
    for(uint i = 0; i < str.size(); i++)
        if((str[i] < '0' || str[i] > '9') && (i != str.size() - 1 || str[i] != '.') && (i != 0 || str[i] != '-'))
            return false;
    return true;
}
