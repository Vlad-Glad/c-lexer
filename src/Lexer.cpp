#include "lexer/Lexer.hpp"
#include <unordered_set>
#include <cctype>
#include <cstring>

namespace clex {

    static const std::unordered_set<std::string> kKeywords = {
      "int","char","if","else","for","while","return","void","const","static","struct",
      "union","enum","typedef","sizeof","switch","case","default","break","continue",
      "do","goto","volatile","extern","signed","unsigned","short","long","float","double",
      "auto","_Bool","_Alignas","_Atomic","_Generic","_Noreturn","_Static_assert","_Thread_local"
    };

    static Token make(TokenKind k, std::string_view slice, int L, int C, std::string msg = {}) {
        return Token{ k, std::string(slice), {L,C}, std::move(msg) };
    }

    Lexer::Lexer(std::string source, LexerOptions opts)
        : s_(std::move(source)), opts_(opts) {
    }

    bool Lexer::eof() const { return i_ >= s_.size(); }

    char Lexer::peek(int k) const {
        return (i_ + k < s_.size()) ? s_[i_ + k] : '\0';
    }

    char Lexer::get() {
        char c = peek();
        if (!eof()) {
            ++i_;
            if (c == '\n') { ++line_; col_ = 1; lineStart_ = i_; }
            else ++col_;
        }
        return c;
    }

    bool Lexer::match(std::string_view t) {
        for (size_t k = 0; k < t.size(); ++k) if (peek((int)k) != t[k]) return false;
        for (size_t k = 0; k < t.size(); ++k) get();
        return true;
    }

    bool Lexer::atLineStart() const { return i_ == lineStart_; }

    bool Lexer::isIdentStart(char c) const {
        return std::isalpha((unsigned char)c) || c == '_';
    }
    bool Lexer::isIdentCont(char c) const {
        return isIdentStart(c) || std::isdigit((unsigned char)c);
    }

    void Lexer::skipWhitespace() {
        while (!eof()) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') get();
            else break;
        }
    }

    // ---- читач≥ категор≥й ----

    Token Lexer::readLineComment() {
        int L = line_, C = col_ - 2; size_t st = i_ - 2;
        while (!eof() && peek() != '\n') get();
        return make(TokenKind::Comment, std::string_view(s_).substr(st, i_ - st), L, C);
    }

    Token Lexer::readBlockComment() {
        int L = line_, C = col_ - 2; size_t st = i_ - 2;
        while (!eof()) {
            if (match("*/"))
                return make(TokenKind::Comment, std::string_view(s_).substr(st, i_ - st), L, C);
            get();
        }
        return make(TokenKind::Error, std::string_view(s_).substr(st, i_ - st), L, C, "Unterminated block comment");
    }

    Token Lexer::readPreproc() {
        int L = line_, C = col_; size_t st = i_;
        while (!eof()) {
            char c = get();
            if (c == '\\' && peek() == '\n') { get(); continue; } // продовженн€ р€дка
            if (c == '\n') break;
        }
        return make(TokenKind::Preprocessor, std::string_view(s_).substr(st, i_ - st), L, C);
    }

    Token Lexer::readString() {
        int L = line_, C = col_; size_t st = i_;
        get(); // consume opening "
        while (!eof()) {
            char c = get();
            if (c == '"') break;
            if (c == '\\' && !eof()) get();   // escape-посл≥довн≥сть
            if (c == '\n') return make(TokenKind::Error, std::string_view(s_).substr(st, i_ - st), L, C, "Unterminated string");
        }
        return make(TokenKind::StringLiteral, std::string_view(s_).substr(st, i_ - st), L, C);
    }

    Token Lexer::readChar() {
        int L = line_, C = col_; size_t st = i_;
        get(); // consume opening '
        bool got = false;
        while (!eof()) {
            char c = get();
            if (c == '\'') { if (!got) return make(TokenKind::Error, std::string_view(s_).substr(st, i_ - st), L, C, "Empty char literal"); break; }
            if (c == '\\' && !eof()) get();
            got = true;
            if (c == '\n') return make(TokenKind::Error, std::string_view(s_).substr(st, i_ - st), L, C, "Unterminated char literal");
        }
        return make(TokenKind::CharLiteral, std::string_view(s_).substr(st, i_ - st), L, C);
    }

    Token Lexer::readNumber() {
        int L = line_, C = col_; size_t st = i_;
        bool isFloat = false;

        // ш≥стнадц€тков≥
        if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
            get(); get();
            bool any = false;
            while (std::isxdigit((unsigned char)peek())) { any = true; get(); }
            if (!any) return make(TokenKind::Error, std::string_view(s_).substr(st, i_ - st), L, C, "Expected hex digits after 0x");
            while (std::strchr("uUlL", peek())) get();
            return make(TokenKind::IntLiteral, std::string_view(s_).substr(st, i_ - st), L, C);
        }

        // дес€тков≥/float
        while (std::isdigit((unsigned char)peek())) get();
        if (peek() == '.') { isFloat = true; get(); while (std::isdigit((unsigned char)peek())) get(); }
        if (peek() == 'e' || peek() == 'E') {
            isFloat = true; get();
            if (peek() == '+' || peek() == '-') get();
            bool any = false; while (std::isdigit((unsigned char)peek())) { any = true; get(); }
            if (!any) return make(TokenKind::Error, std::string_view(s_).substr(st, i_ - st), L, C, "Malformed exponent");
        }
        if (isFloat) {
            if (std::strchr("fFlL", peek())) get();
            return make(TokenKind::FloatLiteral, std::string_view(s_).substr(st, i_ - st), L, C);
        }
        else {
            while (std::strchr("uUlL", peek())) get();
            return make(TokenKind::IntLiteral, std::string_view(s_).substr(st, i_ - st), L, C);
        }
    }

    Token Lexer::readIdentOrKeyword() {
        int L = line_, C = col_; size_t st = i_;
        get();
        while (isIdentCont(peek())) get();
        std::string w = s_.substr(st, i_ - st);
        if (kKeywords.count(w)) return Token{ TokenKind::Keyword, std::move(w), {L,C}, {} };
        return Token{ TokenKind::Identifier, std::move(w), {L,C}, {} };
    }

    Token Lexer::readOperatorOrPunct() {
        int L = line_, C = col_; size_t st = i_;
        auto sv = std::string_view(s_).substr(i_);
        auto take = [&](std::string_view t)->bool {
            if (sv.rfind(t, 0) == 0) { i_ += t.size(); col_ += (int)t.size(); return true; }
            return false;
            };
        // трисимвольн≥
        if (take("...")) return make(TokenKind::Ellipsis, std::string_view(s_).substr(st, i_ - st), L, C);
        if (take(">>=") || take("<<=") || take("->") || take("##"))
            return make(TokenKind::Operator, std::string_view(s_).substr(st, i_ - st), L, C);

        // двосимвольн≥
        static const char* twoOps[] = {
          "==","!=",">=","<=","&&","||","++","--",
          "+=","-=","*=","/=","%=","&=","|=","^=",
          "<<",">>"
        };
        for (auto* op : twoOps)
            if (take(op)) return make(TokenKind::Operator, std::string_view(s_).substr(st, i_ - st), L, C);

        // односимвольн≥
        char c = peek();
        const std::string one = "+-*/%=!<>&|^~?:.,;(){}[]#";
        if (one.find(c) != std::string::npos) {
            get();
            if (c == '#') return make(TokenKind::MacroHash, std::string_view(s_).substr(st, i_ - st), L, C);
            if (std::string("(),;{}[]").find(c) != std::string::npos)
                return make(TokenKind::Punctuator, std::string_view(s_).substr(st, i_ - st), L, C);
            return make(TokenKind::Operator, std::string_view(s_).substr(st, i_ - st), L, C);
        }

        // нев≥домий символ
        char bad = peek();
        get();
        return Token{ TokenKind::Error, std::string(1,bad), {L,C}, "Unknown token" };
    }

    // ---- головн≥ методи ----

    Token Lexer::next() {
        skipWhitespace();
        if (eof()) return Token{ TokenKind::EndOfFile, "", {line_, col_}, {} };

        if (atLineStart() && peek() == '#' && opts_.mergePreproc) return readPreproc();
        if (match("//")) return readLineComment();
        if (match("/*")) return readBlockComment();

        char c = peek();
        if (c == '"') return readString();
        if (c == '\'') return readChar();
        if (std::isdigit((unsigned char)c) || (c == '.' && std::isdigit((unsigned char)peek(1))))
            return readNumber();
        if (isIdentStart(c)) return readIdentOrKeyword();
        return readOperatorOrPunct();
    }

    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> out;
        while (true) {
            auto t = next();
            if (t.kind == TokenKind::Comment && !opts_.keepComments) continue;
            out.push_back(std::move(t));
            if (out.back().kind == TokenKind::EndOfFile || out.back().kind == TokenKind::Error) break;
        }
        return out;
    }
}