#pragma once
#include <string>
#include <vector>
#include <string_view>
#include "Token.hpp"
using namespace std;

namespace clex {

    class Lexer {
    public:
        explicit Lexer(string sourceText);

        vector<Token> TokenizeAll();
        Token GetNextToken();
        bool IsEndOfInput() const;

    private:
        void  AdvanceCursor(string_view matchedLexeme);
        Token MakeToken(TokenKind kind,
            string_view lexeme,
            int lineAtStart,
            int columnAtStart,
            string message = {});

    private:
        string     sourceText_;
        size_t     index_ = 0;
        int        line_ = 1;
        int        column_ = 1;
    };

}