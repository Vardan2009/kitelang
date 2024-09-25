#pragma once
#include <string>

namespace lexer {
	// Enumeration for the type of the token
	typedef enum {
		KEYWORD,
		IDENTIFIER,
		STRING_LIT,
		REG,
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
		EQ,
		CDIRECT,
		ADDROF,
		DEREF,
		EQEQ,
		NEQEQ,
		GT,
		LT,
		GTE,
		LTE
	} token_t;

	// Token class
	class Token {
	public:
		token_t type;
		// each token has either of these values
		// the value of the token is stored in `value` if it is integer, `value_str` if it is string
		int value;
		std::string value_str;
		// constructor with integer value
		Token(token_t type, int value) {
			this->type = type;
			this->value = value;
		}
		// constructor with string value
		Token(token_t type, std::string value_str) {
			this->type = type;
			this->value_str = value_str;
		}
	};
}