module;

#include <iostream>

export module MyModule;

int hidden() {
    return 42;
}

export void printMessage() {
    std::cout << "the hidden value is " << hidden() << std::endl;
}