#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"

using namespace clex;

static void printUsage(const char* argv0) {
	std::cerr << "Usage: " << argv0 << " <path/to/file.c>\n";
 }

static void printToken(const Token& t, bool color) {
    if (color) {
        if (t.kind == TokenKind::Keyword) std::cout << "\x1b[34m";
        else if (t.kind == TokenKind::IntLiteral || t.kind == TokenKind::FloatLiteral) std::cout << "\x1b[36m";
        else if (t.kind == TokenKind::StringLiteral || t.kind == TokenKind::CharLiteral) std::cout << "\x1b[32m";
        else if (t.kind == TokenKind::Comment) std::cout << "\x1b[90m";
        else if (t.kind == TokenKind::Error) std::cout << "\x1b[31m";
    }
    std::cout << "<" << t.lexeme << ", " << to_string(t.kind)
        << ", " << t.pos.line << ":" << t.pos.column << ">";
    if (!t.message.empty()) std::cout << " // " << t.message;
    if (color) std::cout << "\x1b[0m";
    std::cout << "\n";
}


int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    bool keepComments = true;
    bool color = true;

    for (int i = 2; i < argc; ++i) {
        std::string opt = argv[i];
        if (opt == "--no-comments") keepComments = false;
        else if (opt == "--no-color") color = false;
    }

    std::ifstream in(argv[1]);

    if (!in) { 
        std::cerr << "Cannot open: " << argv[1] << "\n"; return 1; 
    }

    std::stringstream buf;
    buf << in.rdbuf();

    Lexer lex(buf.str(), LexerOptions{ keepComments, /*mergePreproc*/ true });
    auto tokens = lex.tokenize();

    bool hasError = false;
    for (const auto& t : tokens) {
        printToken(t, color);          // <-- виправили 'color'
        if (t.kind == TokenKind::Error) hasError = true;
    }
    return hasError ? 2 : 0;
}