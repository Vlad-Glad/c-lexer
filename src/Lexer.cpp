#include "lexer/Lexer.hpp"

namespace clex {

	Lexer::Lexer(std::string source, LexerOptions opts)
		: s_(std::move(source)), opts_(opts) {
	}

	bool Lexer::eof() const { return i_ >= s_.size(); }

	std::vector<Token> Lexer::tokenize() {
		return 
		{ 
			Token{TokenKind::EndOfFile, "", {line_, col_}, {}}
		};
	}

	Token Lexer::next() {
		return Token
		{ 
			TokenKind::EndOfFile, "", {line_, col_}, {}
		};
	}
}