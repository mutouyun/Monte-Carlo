#include <iostream>
using namespace std;

#include "five.h"

int counter__ = 0;

int main() {
    five_prepare(500000, [](std::size_t t) {
        cout << "games[" << ++counter__ << "]: " << t << endl;
    });
    return 0;
}
