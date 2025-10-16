#include "lexer/Lexer.hpp"
#include <regex>
#include <unordered_set>
#include <cctype>
using namespace std;

namespace clex {

    namespace {

        const unordered_set<string> kKeywords = {
          "int","char","float","double","void","if","else","for","while","return",
          "const","static","struct","union","enum","typedef","sizeof",
          "switch","case","default","break","continue","do","goto",
          "volatile","extern","signed","unsigned","short","long","auto","_Bool"
        };

        const regex RX_WS(R"(^[ \t\r\n]+)");
        const regex RX_PREPROC(R"(^#[^\n]*(\\\n[^\n]*)*)");

        const regex RX_LINE_COM(R"(^//[^\n]*)");
        const regex RX_BLOCK_COM(R"(^/\*[\s\S]*?\*/)");

        const regex RX_STRING(R"(^"([^"\\]|\\.)*")");
        const regex RX_CHAR(R"(^'([^'\\]|\\.)')");

        const regex RX_INT_HEX(R"(^0[xX][0-9A-Fa-f]+[uUlL]*)");
        const regex RX_FLOAT(R"(^(([0-9]+\.[0-9]*|\.[0-9]+|[0-9]+([eE][+\-]?[0-9]+)))[fFlL]?)");
        const regex RX_INT_DEC(R"(^[0-9]+[uUlL]*)");

        const regex RX_IDENT(R"(^[A-Za-z_][A-Za-z0-9_]*)");

        const regex RX_OP_ALL(
            R"(^(\.{3}|>>=|<<=|==|!=|>=|<=|&&|\|\||\+\+|--|\+=|-=|\*=|/=|%=|&=|\|=|\^=|<<|>>|->|##|[+\-*/%=!<>&|^~?:.,;(){}\[\]#]))"
        );

        template <class Re>
        bool MatchAtBegin(string_view sv, const Re& re, string& out) {
            match_results<string_view::const_iterator> m;

            if (regex_search(sv.begin(), sv.end(), m, re,
                regex_constants::match_continuous)) {
                out.assign(m[0].first, m[0].second);
                return true;
            }

            return false;
        }

    } 

    Lexer::Lexer(string sourceText)
        : sourceText_(move(sourceText)) {
    }

    bool Lexer::IsEndOfInput() const { 
        return index_ >= sourceText_.size(); 
    }

    void Lexer::AdvanceCursor(string_view matchedLexeme) {
        for (char c : matchedLexeme) {
            ++index_;
            if (c == '\n') { ++line_; column_ = 1; }
            else { ++column_; }
        }
    }

    Token Lexer::MakeToken(TokenKind kind,
        string_view lexeme,
        int lineAtStart,
        int columnAtStart,
        string message) {
        return Token{ kind, string(lexeme), { lineAtStart, columnAtStart }, move(message) };
    }

    Token Lexer::GetNextToken() {
        while (!IsEndOfInput()) {
            string_view sv = string_view(sourceText_).substr(index_);
            int L = line_, C = column_;
            string m;

            if (MatchAtBegin(sv, RX_WS, m)) { AdvanceCursor(m); continue; }

            if (MatchAtBegin(sv, RX_PREPROC, m)) { 
                AdvanceCursor(m); 
                return MakeToken(TokenKind::Preprocessor, m, L, C); 
            }

            if (MatchAtBegin(sv, RX_LINE_COM, m)) { 
                AdvanceCursor(m); 
                return MakeToken(TokenKind::Comment, m, L, C); 
            }

            if (MatchAtBegin(sv, RX_BLOCK_COM, m)) { 
                AdvanceCursor(m); 
                return MakeToken(TokenKind::Comment, m, L, C); 
            }

            if (sv.rfind("/*", 0) == 0) { // unterminated block comment
                AdvanceCursor(sv);
                return MakeToken(TokenKind::Error, sv, L, C, "Unterminated block comment");
            }

            if (!sv.empty() && sv[0] == '"') {
                if (MatchAtBegin(sv, RX_STRING, m)) { 
                    AdvanceCursor(m); 
                    return MakeToken(TokenKind::StringLiteral, m, L, C); 
                }

                size_t len = 1; 
                while (len < sv.size() && sv[len] != '\n') ++len;

                string_view bad = sv.substr(0, len); 
                AdvanceCursor(bad);

                return MakeToken(TokenKind::Error, bad, L, C, "Unterminated string");
            }
            if (!sv.empty() && sv[0] == '\'') {
                if (MatchAtBegin(sv, RX_CHAR, m)) { 
                    AdvanceCursor(m); 
                    return MakeToken(TokenKind::CharLiteral, m, L, C); 
                }

                size_t len = 1; 
                while (len < sv.size() && sv[len] != '\n') ++len;

                string_view bad = sv.substr(0, len); 
                AdvanceCursor(bad);

                return MakeToken(TokenKind::Error, bad, L, C, "Unterminated char literal");
            }

            if (MatchAtBegin(sv, RX_INT_HEX, m)) { 
                AdvanceCursor(m); 
                return MakeToken(TokenKind::IntLiteral, m, L, C); 
            }

            if (MatchAtBegin(sv, RX_FLOAT, m)) { 
                AdvanceCursor(m); 
                return MakeToken(TokenKind::FloatLiteral, m, L, C); 
            }

            if (MatchAtBegin(sv, RX_INT_DEC, m)) { 
                AdvanceCursor(m); 
                return MakeToken(TokenKind::IntLiteral, m, L, C); 
            }

            if (MatchAtBegin(sv, RX_IDENT, m)) {
                AdvanceCursor(m);

                if (kKeywords.count(m)) {
                    return Token{ TokenKind::Keyword, m, { L, C }, {} };
                }

                return Token{ TokenKind::Identifier, m, { L, C }, {} };
            }

            if (MatchAtBegin(sv, RX_OP_ALL, m)) {
                if (m == "...") { 
                    AdvanceCursor(m); 
                    return MakeToken(TokenKind::Ellipsis, m, L, C); 
                }

                if (m == "##") { 
                    AdvanceCursor(m); 
                    return MakeToken(TokenKind::MacroConcat, m, L, C); 
                }

                if (m == "#") { 
                    AdvanceCursor(m); 
                    return MakeToken(TokenKind::MacroHash, m, L, C); 
                }

                if (m.size() == 1 && string("(),;{}[]").find(m[0]) != string::npos) {
                    AdvanceCursor(m); 
                    return MakeToken(TokenKind::Punctuator, m, L, C);
                }

                AdvanceCursor(m); return MakeToken(TokenKind::Operator, m, L, C);
            }

            string one(1, sv[0]);
            AdvanceCursor(one);
            return MakeToken(TokenKind::Error, one, L, C, "Unknown token");
        }

        return Token{ TokenKind::EndOfFile, "", { line_, column_ }, {} };
    }

    vector<Token> Lexer::TokenizeAll() {
        vector<Token> out;

        while (true) {
            Token t = GetNextToken();
            out.push_back(move(t));

            if (out.back().kind == TokenKind::EndOfFile || out.back().kind == TokenKind::Error) break;
        }
        return out;
    }

}
