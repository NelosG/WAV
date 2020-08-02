#include "WAV.h"
#include <string>
#include <iostream>



int main(int argc, char** argv) {
    WAV wav;
    if (argc < 2) wav.read();
    else wav.read(argv[1]);
    std::string s;
    while (true) {
        std::cout << R"(Enter command ("help" for all commands): )";
        std::cin >> s;
        std::cout << '\n';
        if (s == "cut") wav.cut();
        else if (s == "write") wav.write();
        else if (s == "read") wav.read();
        else if (s == "end") break;
        else if (s == "help") {
            std::cout << "\n    \"cut <start> <end>\" for cut readed file (format: 1.20 equals 1 min 20 sec)\n";
            std::cout <<   "    \"read <filename>\" for read file (you need to specify the full path to the file)\n";
            std::cout <<   "    \"write <filename>\" for write current file (you need to specify the full path to the file)\n";
            std::cout <<   "    \"end\" to end the program\n\n\n";
        }
        else std::cout << R"(Command not recognized. Type "help" for a list of commands)" << '\n\n';
    }
    return 0;
}

