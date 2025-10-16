#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


#include "lexer/Token.hpp"

static void printUsage(const char* argv0) {
	std::cerr << "Usage: " << argv0 << " <path/to/file.c>\n";
 }

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string path = argv[1];

    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Error: cannot open file: " << path << "\n";
        return 1;
    }

    std::ostringstream buf;
    buf << in.rdbuf();
    std::string source = buf.str();

    std::size_t bytes = source.size();

    std::size_t lines = 0;
    for (char c : source) if (c == '\n') ++lines;
    if (!source.empty() && source.back() != '\n') ++lines;

    std::cout << "Loaded file: " << path << "\n";
    std::cout << "Bytes: " << bytes << "\n";
    std::cout << "Lines: " << lines << "\n";


    std::cout << "---- preview (up to 10 lines) ----\n";
    {
        std::istringstream inmem(source);
        std::string line;
        int shown = 0;
        while (shown < 10 && std::getline(inmem, line)) {
            std::cout << (shown + 1) << ": " << line << "\n";
            ++shown;
        }
        if (shown == 0) std::cout << "(file is empty)\n";
        if (shown == 10) std::cout << "... (truncated)\n";
    }
    std::cout << "-----------------------------------\n";


    return 0;
}