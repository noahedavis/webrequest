#include "Print.h"

using std::cout;
using std::to_string;

void print(string str) {
    cout << str;
}

void println(string str) {
    string prnt = str + "\n";
    cout << prnt;
}

void print(int val) {
    print(to_string(val));
}

void println(int val) {
    println(to_string(val));
}