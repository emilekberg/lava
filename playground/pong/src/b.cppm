module;

#include <iostream>

export module MyModuleB;
// import MyModule;

int hidden() {
    return 69;
}

export void printMessageB() {
    // printMessage();
    std::cout << "the hidden value is " << hidden() << std::endl;
}