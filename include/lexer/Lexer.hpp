#pragma once
#include <string>
#include <vector>
#include <string_view>
#include "Token.hpp"

namespace clex {

	class Lexer {
	public:
		explicit Lexer(std::string source);
		std::vector<Token> tokenize();
		Token next();
		bool eof() const;

	private:
		// helpers
		void advance(std::string_view s); // move i_/line_/col_
		Token make(TokenKind k, std::string_view m, int L, int C, std::string msg = {});

	private: 
		std::string s_;
		std::size_t i_ = 0;
		int line_ = 1, col_ = 1;
	};
} 