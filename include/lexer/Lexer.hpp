#pragma once
#include <string>
#include <vector>
#include <string_view>
#include "Token.hpp"

namespace clex {

	struct LexerOptions {
		bool keepComments = true; // зберігати коментарі у вихідному потоці токенів
		bool mergePreproc = true; // директиву препроцесора давати одним токеном
	};

	class Lexer {
	public:
		explicit Lexer(std::string source, LexerOptions opts = {});

		std::vector<Token> tokenize();
		Token next();
		bool eof() const;

	private:
		Token readPreproc();
		Token readLineComment();
		Token readBlockComment();
		Token readString();
		Token readChar();
		Token readNumber();
		Token readIdentOrKeyword();
		Token readOperatorOrPunct();
		void  skipWhitespace();

		char  peek(int k = 0) const;
		char  get();
		bool  match(std::string_view t);
		bool  atLineStart() const;
		bool  isIdentStart(char c) const;
		bool  isIdentCont(char c) const;

	private:
		std::string s_;
		std::size_t i_ = 0;
		int line_ = 1, col_ = 1;
		std::size_t lineStart_ = 0;
		LexerOptions opts_;
	};

}