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
		LTE,
		LSQR,
		RSQR,
		COLON,
		CARET,
		ARROW,
		MOD
	} token_t;

	// Token class
	class Token {
	public:
		token_t type;
		// the the line where the token is, and the range in the line
		int line, pos_start, pos_end;
		// each token has either of these values
		// the value of the token is stored in `value` if it is integer, `value_str` if it is string
		int value;
		std::string value_str;
		// constructor with integer value
		Token(token_t type, int value, int line, int pos_start, int pos_end) {
			this->type = type;
			this->value = value;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		// constructor with string value
		Token(token_t type, std::string value_str, int line, int pos_start, int pos_end) {
			this->value = -1;
			this->type = type;
			this->value_str = value_str;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
	};
}