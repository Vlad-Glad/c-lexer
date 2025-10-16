#pragma once
#include <string>
using namespace std;

namespace clex {

	enum class TokenKind {
		Identifier, Keyword, IntLiteral, FloatLiteral, CharLiteral, StringLiteral,
		Preprocessor, Comment, Operator, Punctuator, Ellipsis, MacroHash, MacroConcat,
		Error, EndOfFile
	};

	struct SourcePos { int line = 1; int column = 1; };

	struct Token {
		TokenKind   kind{};
		string      lexeme;
		SourcePos   pos{};
		string      message; 
	};

	string to_string(TokenKind);

} 
