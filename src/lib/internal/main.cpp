#include <iostream>

#include "registrator.h"

int main() {
    try {
        dbuser::Register r("org.firsov.server");
        r.startLoop();
    } catch (const dbuser::RegistrationError &err) {
        std::cerr << "Connection error: " << err.what() << "\n";
        return 1;
    }
    return 0;
}
