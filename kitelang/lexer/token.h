#pragma once
#include <string>

namespace lexer {
	typedef enum {
		KEYWORD,
		IDENTIFIER,
		STRING_LIT,
		INT_LIT,
		CHAR_LIT,
		LBRACE,
		RBRACE,
		LPAREN,
		RPAREN,
		PLUS,
		MINUS,
		MUL,
		DIV,
		COMMA,
		EQ
	} token_t;

	class Token {
	public:
		token_t type;
		int value;
		std::string value_str;
		Token(token_t type, int value) {
			this->type = type;
			this->value = value;
		}
		Token(token_t type, std::string value_str) {
			this->type = type;
			this->value_str = value_str;
		}
	};
}