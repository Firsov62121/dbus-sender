#include <iostream>
#include <cstring>

#include "sender.h"


static int bad_finish() {
    std::cout << "Syntax: sender [reg|open] param1 [param2]\n";
    return 1;
}

int main(int argc, const char** argv) {
    if (argc > 4 || argc == 1)
        return bad_finish();
    try {
        if (strcmp(argv[1], "reg") == 0) {
            if (argc != 4)
                return bad_finish();
            auto res = sender::add_file(argv[2], argv[3]);
            std::cout << "result = " << res << "\n";
        } else if (strcmp(argv[1], "open") == 0) {
            if (argc != 3)
                return bad_finish();
            sender::open_file(argv[2]);
        } else {
            return bad_finish();
        }
    } catch (const sender::SenderError &err) {
        std::cerr << err.what() << "\n";
    }
    return 0;
}
