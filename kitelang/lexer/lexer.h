#pragma once
#include "token.h"
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>
#include <algorithm>

typedef std::shared_ptr<lexer::Token> token_ptr;

namespace lexer {
	class Lexer {
	private:
		std::map<char, lexer::token_t> specials = {
			{'+', lexer::PLUS},
			{'-', lexer::MINUS},
			{'*', lexer::MUL},
			{'/', lexer::DIV},
			{'(', lexer::LPAREN},
			{')', lexer::RPAREN},
			{'{', lexer::LBRACE},
			{'}', lexer::RBRACE},
			{',', lexer::COMMA},
		};
		std::set<std::string> keywords = {
			"extern", "global", "routine", "call"
		};
		std::string src;
		int ptr = 0;
		token_ptr make_int();
		token_ptr make_string();
		token_ptr make_identifier();
		token_ptr make_special();
		void skip_comment();
	public:
		Lexer(std::string src) {
			this->src = src;
			this->ptr = 0;
		}
		std::vector<token_ptr> tokenize();
	};
}