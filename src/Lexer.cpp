#include "lexer/Lexer.hpp"
#include <regex>
#include <unordered_set>
#include <cctype>

namespace clex {

    static const std::unordered_set<std::string> kKeywords = {
      "int","char","float","double","void","if","else","for","while","return",
      "const","static","struct","union","enum","typedef","sizeof",
      "switch","case","default","break","continue","do","goto",
      "volatile","extern","signed","unsigned","short","long","auto","_Bool"
    };

    static const std::regex RX_WS(R"(^[ \t\r\n]+)");
    static const std::regex RX_PREPROC(R"(^#[^\n]*(\\\n[^\n]*)*)");

    static const std::regex RX_LINE_COM(R"(^//[^\n]*)");
    static const std::regex RX_BLOCK_COM(R"(^/\*[\s\S]*?\*/)");

    static const std::regex RX_STRING(R"(^"([^"\\]|\\.)*")");
    static const std::regex RX_CHAR(R"(^'([^'\\]|\\.)')");

    static const std::regex RX_INT_HEX(R"(^0[xX][0-9A-Fa-f]+[uUlL]*)");

    static const std::regex RX_FLOAT(
        R"(^(([0-9]+\.[0-9]*|\.[0-9]+|[0-9]+([eE][+\-]?[0-9]+)))[fFlL]?)"
    );

    static const std::regex RX_INT_DEC(R"(^[0-9]+[uUlL]*)");
    static const std::regex RX_IDENT(R"(^[A-Za-z_][A-Za-z0-9_]*)");

    static const std::regex RX_OP_ALL(
        R"(^(\.{3}|>>=|<<=|==|!=|>=|<=|&&|\|\||\+\+|--|\+=|-=|\*=|/=|%=|&=|\|=|\^=|<<|>>|->|##|[+\-*/%=!<>&|^~?:.,;(){}\[\]#]))"
    );


    // ---- спільний matcher ----
    template<class Re>
    static bool match_begin(std::string_view sv, const Re& re, std::string& out) {
        std::match_results<std::string_view::const_iterator> m;
        if (std::regex_search(sv.begin(), sv.end(), m, re,
            std::regex_constants::match_continuous)) {
            out.assign(m[0].first, m[0].second);
            return true;
        }
        return false;
    }

    Lexer::Lexer(std::string source) : s_(std::move(source)) {}

    bool Lexer::eof() const { return i_ >= s_.size(); }

    void Lexer::advance(std::string_view text) {
        for (char c : text) {
            ++i_;
            if (c == '\n') { ++line_; col_ = 1; }
            else { ++col_; }
        }
    }

    Token Lexer::make(TokenKind k, std::string_view m, int L, int C, std::string msg) {
        return Token{ k, std::string(m), {L, C}, std::move(msg) };
    }

    Token Lexer::next() {
        while (!eof()) {
            std::string_view sv = std::string_view(s_).substr(i_);
            int L = line_, C = col_;
            std::string m;

            // 1) пропустити пробіли
            if (match_begin(sv, RX_WS, m)) { advance(m); continue; }

            // 2) препроцесор (#... із переносами рядка) — одним токеном
            if (match_begin(sv, RX_PREPROC, m)) { advance(m); return make(TokenKind::Preprocessor, m, L, C); }

            // 3) коментарі (всегда повертаємо як токени)
            if (match_begin(sv, RX_LINE_COM, m)) { advance(m); return make(TokenKind::Comment, m, L, C); }
            if (match_begin(sv, RX_BLOCK_COM, m)) { advance(m); return make(TokenKind::Comment, m, L, C); }
            // якщо починається на "/*", але не збіглося — незакритий блочний
            if (sv.rfind("/*", 0) == 0) { advance(sv); return make(TokenKind::Error, sv, L, C, "Unterminated block comment"); }

            // 4) рядки / символи
            if (!sv.empty() && sv[0] == '"') {
                if (match_begin(sv, RX_STRING, m)) { advance(m); return make(TokenKind::StringLiteral, m, L, C); }
                std::size_t len = 1; while (len < sv.size() && sv[len] != '\n') ++len;
                std::string_view bad = sv.substr(0, len); advance(bad);
                return make(TokenKind::Error, bad, L, C, "Unterminated string");
            }
            if (!sv.empty() && sv[0] == '\'') {
                if (match_begin(sv, RX_CHAR, m)) { advance(m); return make(TokenKind::CharLiteral, m, L, C); }
                std::size_t len = 1; while (len < sv.size() && sv[len] != '\n') ++len;
                std::string_view bad = sv.substr(0, len); advance(bad);
                return make(TokenKind::Error, bad, L, C, "Unterminated char literal");
            }

            // 5) числа: hex → float → десяткові int
            if (match_begin(sv, RX_INT_HEX, m)) { advance(m); return make(TokenKind::IntLiteral, m, L, C); }
            if (match_begin(sv, RX_FLOAT, m)) { advance(m); return make(TokenKind::FloatLiteral, m, L, C); }
            if (match_begin(sv, RX_INT_DEC, m)) { advance(m); return make(TokenKind::IntLiteral, m, L, C); }

            // 6) ідентифікатори / ключові слова
            if (match_begin(sv, RX_IDENT, m)) {
                advance(m);
                if (kKeywords.count(m)) return Token{ TokenKind::Keyword, m, {L,C}, {} };
                return Token{ TokenKind::Identifier, m, {L,C}, {} };
            }

            // 7) оператори / пунктуація / спец
            if (match_begin(sv, RX_OP_ALL, m)) {
                if (m == "...") { advance(m); return make(TokenKind::Ellipsis, m, L, C); }
                if (m == "##") { advance(m); return make(TokenKind::MacroConcat, m, L, C); }
                if (m == "#") { advance(m); return make(TokenKind::MacroHash, m, L, C); }
                if (m.size() == 1 && std::string("(),;{}[]").find(m[0]) != std::string::npos) {
                    advance(m); return make(TokenKind::Punctuator, m, L, C);
                }
                advance(m); return make(TokenKind::Operator, m, L, C);
            }

            // 8) невідомий символ → помилка (1 символ)
            std::string one(1, sv[0]);
            advance(one);
            return make(TokenKind::Error, one, L, C, "Unknown token");
        }

        return Token{ TokenKind::EndOfFile, "", {line_, col_}, {} };
    }

    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> out;
        while (true) {
            Token t = next();
            out.push_back(std::move(t));
            if (out.back().kind == TokenKind::EndOfFile || out.back().kind == TokenKind::Error) break;
        }
        return out;
    }

} 