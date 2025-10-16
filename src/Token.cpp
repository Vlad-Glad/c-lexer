#include "lexer/Token.hpp"
using namespace std;

namespace clex {
    string to_string(TokenKind k) {
        switch (k) {
        case TokenKind::Identifier:   return "Identifier";
        case TokenKind::Keyword:      return "Keyword";
        case TokenKind::IntLiteral:   return "IntLiteral";
        case TokenKind::FloatLiteral: return "FloatLiteral";
        case TokenKind::CharLiteral:  return "CharLiteral";
        case TokenKind::StringLiteral:return "StringLiteral";
        case TokenKind::Preprocessor: return "Preprocessor";
        case TokenKind::Comment:      return "Comment";
        case TokenKind::Operator:     return "Operator";
        case TokenKind::Punctuator:   return "Punctuator";
        case TokenKind::Ellipsis:     return "Ellipsis";
        case TokenKind::MacroHash:    return "MacroHash";
        case TokenKind::MacroConcat:  return "MacroConcat";
        case TokenKind::Error:        return "Error";
        case TokenKind::EndOfFile:    return "EOF";
        }
        return "Unknown";
    }
} 
