#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"

using namespace clex;

static void printUsage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " <file.c>\n";
}

static void printToken(const Token& t) {
    std::cout << "<" << t.lexeme << ", " << to_string(t.kind)
        << ", " << t.pos.line << ":" << t.pos.column << ">";

    if (!t.message.empty()) std::cout << " // " << t.message;
    std::cout << "\n";
}

int main(int argc, char** argv) {
    if (argc < 2) { printUsage(argv[0]); return 1; }

    std::ifstream in(argv[1], std::ios::binary);
    if (!in) { 
        std::cerr << "Cannot open: " << argv[1] << "\n"; return 1; 
    }

    std::ostringstream buf; buf << in.rdbuf();
    Lexer lex(buf.str());

    auto toks = lex.tokenize();
    bool hasErr = false;

    for (const auto& t : toks) { 
        printToken(t); 
        if (t.kind == TokenKind::Error) hasErr = true; 
    }
    return hasErr ? 2 : 0;
}
