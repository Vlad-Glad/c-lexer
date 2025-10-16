#pragma once
#include <string>

namespace clex {

	enum class TokenKind {
		Identifier,
		Keyword,
		IntLiteral,
		FloatLiteral,
		CharLiteral,
		StringLiteral,
		Preprocessor,
		Comment,
		Operator,
		Punctuator,
		Ellipsis,
		MacroHash, 
		MacroConcat,
		Error, 
		EndOfFile
	};

	struct SourcePos {
		int line = 1;
		int column = 1;
	};


	struct Token {
		TokenKind kind{};
		std::string lexeme;
		SourcePos pos{};
		std::string message;
	};

	std::string to_string(TokenKind kind);

}	