#pragma once
#include "token.h"
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>
#include <algorithm>

// define an alias for a shared ptr to token object token_ptr
typedef std::shared_ptr<lexer::Token> token_ptr;

namespace lexer {
	// Lexer class
	class Lexer {
	private:
		// map of each special character to its token type
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
			{'=', lexer::EQ},
		};
		// this is to determine if the "word" is a keyword or a reference to a variable or function (identifier)
		std::set<std::string> keywords = {
			"extern", "global", "fn", "let", "for", "to", "step", "cmp", "asm", "eq", "neq", "return", "break", "continue", "loop"
		};
		std::string src;
		// the pointer to the current character
		int ptr = 0;
		// function declarations
		token_ptr make_int();               // for making and returning an integer token                (e.g `1234`)
		token_ptr make_string();            // for making and returning a string token                  (e.g `"Hello, World!"`)
		token_ptr make_char();              // for making and returning a char token                    (e.g `'A'`)
		token_ptr make_reg();               // for making and returning a register token                (e.g `%rax`)
		token_ptr make_identifier();        // for making and returning a keyword or identifier token   (e.g `varName` or `let`)
		token_ptr make_special();           // for making and returning a special character token       (e.g `+` or `~`)
		void skip_comment();                // for skipping single line comments                        (e.g % test)
		void skip_multiline_comment();      // for skipping multiline comments                          (e.g ~ test ~)
	public:
	    // the constructor that takes the source code and resets the character pointer
		Lexer(std::string src) {
			this->src = src;
			this->ptr = 0;
		}
		// main tokenize function that returns the array of tokens representing the source code
		std::vector<token_ptr> tokenize();
	};
}