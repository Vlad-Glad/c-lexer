#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
using namespace std;

static void PrintUsage(const char* argv0) {
    cerr << "Usage: " << argv0 << " <file.c>\n";
}

static void PrintToken(const clex::Token& t) {
    cout << "<" << t.lexeme << ", " << clex::to_string(t.kind)
        << ", " << t.pos.line << ":" << t.pos.column << ">";

    if (!t.message.empty()) cout << " // " << t.message;

    cout << "\n";
}

int main(int argc, char** argv) {
    if (argc < 2) { 
        PrintUsage(argv[0]); return 1; 
    }

    ifstream in(argv[1], ios::binary);
    if (!in) { 
        cerr << "Cannot open: " << argv[1] << "\n"; return 1; 
    }

    ostringstream buf; 
    buf << in.rdbuf();

    clex::Lexer lexer(buf.str());
    vector<clex::Token> tokens = lexer.TokenizeAll();

    bool hasError = false;
    for (const clex::Token& t : tokens) {
        PrintToken(t);
        if (t.kind == clex::TokenKind::Error) hasError = true;
    }
    return hasError ? 2 : 0;
}
