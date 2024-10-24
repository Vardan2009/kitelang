#pragma once
#include "token.h"
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "../errors/errors.h"

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
			{'>', lexer::GT},
			{'<', lexer::LT},
			{'[', lexer::LSQR},
			{']', lexer::RSQR},
			{']', lexer::RSQR},
			{':', lexer::COLON},
			{'^', lexer::CARET},
			{'%', lexer::MOD},
		};
		// map of each special two characters to its token type
		std::map<std::string, lexer::token_t> specialsTwoChar = {
			{"==", lexer::EQEQ},
			{"!=", lexer::NEQEQ},
			{">=", lexer::GTE},
			{"<=", lexer::LTE},
			{"->", lexer::ARROW},
			{"::", lexer::VAARG},
		};
		// map of each prefix and its token type
		std::map<char, lexer::token_t> prefixes = {
			{'^', lexer::REG},
			{'&', lexer::ADDROF},
			{'*', lexer::DEREF},
			{'@', lexer::CDIRECT}
		};
		// set of keywords in the language
		// this is to determine if the "word" is a keyword or a reference to a variable or function (identifier)
		std::set<std::string> keywords = {
			"extern", "global", "fn", "let", "for", "cmp", "asm", "eq", "neq", "return", "break", "continue", "loop", "if", "else",
			"void", "char", "byte", "bool", "int16","int32", "int64", "ptr8", "ptr16", "ptr32", "ptr64"
		};
		// The current line and position
		int line = 0;
		int pos = 0;
		// The source code
		std::string src;
		// the pointer to the current character
		int ptr = 0;
		// function declarations
		token_ptr make_int();                // for making and returning an integer token                (e.g `1234`)
		token_ptr make_string();             // for making and returning a string token                  (e.g `"Hello, World!"`)
		token_ptr make_char();               // for making and returning a char token                    (e.g `'A'`)
		token_ptr make_identifier();         // for making and returning a keyword or identifier token   (e.g `varName` or `let`)
		token_ptr make_special();            // for making and returning a special character token       (e.g `+` or `~`)
		token_ptr make_special_two();        // for making and returning a special token with two chars  (e.g `==` or `!=`)
		token_ptr make_with_prefix(token_t); // for making and returning an identifier with a prefix     (e.g `^rax` or `*ptr`)
		void skip_comment();                 // for skipping single line comments                        (e.g % test)
		void skip_multiline_comment();       // for skipping multiline comments                          (e.g ~ test ~)
		char advance();						 // for advancing to next character and keeping line and pos count right
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