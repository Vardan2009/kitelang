#include "lexer.h"
#include <iostream>

std::vector<token_ptr> lexer::Lexer::tokenize() {
	// initialize the array (vector)
	std::vector<token_ptr> result;

	// while the pointer is in the bounds of the characters
	while (ptr < src.size()) {
		// if the current character (src[ptr]) is an alphabetic character or an underscore
		// expect and parse the identifier and add to the tokens array
		if (isalpha(src[ptr]) || src[ptr] == '_')
			result.push_back(make_identifier());
		// if the current character is a digit, parse an integer
		// floating point numbers are not yet implemented in the language
		else if (isdigit(src[ptr]))
			result.push_back(make_int());
		// if the current character is a double quote, that means it's a string literal
		else if (src[ptr] == '"')
			result.push_back(make_string());
		// if the current character is a single quote, that means it's a char literal
		else if (src[ptr] == '\'')
			result.push_back(make_char());
		// if the current character is an and sign, it is a register reference (e.g &rax)
		else if (src[ptr] == '&')
			result.push_back(make_reg());
		// if the current character is an at sign, it is a compiler directive (e.g @stackszinc)
		else if (src[ptr] == '@')
			result.push_back(make_cdirect());
		// if the count of the current character is non-zero (it is in the list)
		// then it is a special character
		else if (specials.count(src[ptr]))
			result.push_back(make_special());
		// if it is a single line comment prefix, skip the comment
		else if (src[ptr] == '%') skip_comment();
		// if it is a multiline comment prefix, skip the comment
		else if (src[ptr] == '~') skip_multiline_comment();
		// ignore whitespace
		else if (isspace(src[ptr])) ++ptr;
		// case for invalid characters
		else
			throw std::runtime_error("invalid character `" + std::to_string(src[ptr]) + "`");
	}
	return result;
}

token_ptr lexer::Lexer::make_identifier() {
	// this will store the result
	std::string result;

	// while the current character is alphanumeric or an underscore
	while (isalnum(src[ptr]) || src[ptr] == '_') {
		// add to the result and increment the pointer
		result += src[ptr++];
	}

	// determine if the "word" is an identifier or a keyword
	// if the result is in the "keywords" list (in lexer.h), then it is a keyword
	token_t type =
		(keywords.find(result) != keywords.end())
		? KEYWORD
		: IDENTIFIER;

	return std::make_shared<Token>(type, result);
}

token_ptr lexer::Lexer::make_int() {
	// this will store the result (as a string for now)
	std::string result;

	// while it is a digit, add to the result and increment the pointer
	while (isdigit(src[ptr])) {
		result += src[ptr++];
	}

	// convert the result to integer (atoi) and return
	return std::make_shared<Token>(INT_LIT, std::atoi(result.c_str()));
}

token_ptr lexer::Lexer::make_string() {
	// skip through the double quote
	ptr++;

	// this will store the result
	std::string result;

	// while the current character is not a closing quote
	while (src[ptr] != '"') {
		if (src[ptr] == '\\')
			result += src[ptr++];
		result += src[ptr++];
	}

	// skip through the closing double quote
	ptr++;
	return std::make_shared<Token>(STRING_LIT, result);
}

token_ptr lexer::Lexer::make_cdirect() {
	// skip through the single quote
	ptr++;

	// this will store the result
	std::string result;

	// while the current character is alphanumeric or an underscore
	while (isalnum(src[ptr])) {
		// add to the result and increment the pointer
		result += src[ptr++];
	}

	return std::make_shared<Token>(CDIRECT, result);
}

token_ptr lexer::Lexer::make_reg() {
	// skip through the single quote
	ptr++;

	// this will store the result
	std::string result;

	// while the current character is alphanumeric or an underscore
	while (isalnum(src[ptr])) {
		// add to the result and increment the pointer
		result += src[ptr++];
	}

	return std::make_shared<Token>(REG, result);
}

token_ptr lexer::Lexer::make_char() {
	// skip through the single quote
	ptr++;
	// get the result and increment, since it is guaranteed to be a single character
	char result = src[ptr++];
	if (result == '\\') {
		char code = src[ptr++];
		switch (code) {
		case 'n':
			result = '\n';
			break;
		case '0':
			result = '\0';
			break;
		case 't':
			result = '\t';
			break;
		case 'r':
			result = '\r';
			break;
		case '\\':
			result = '\\';
			break;
		case '\"':
			result = '\"';
			break;
		default:
			throw std::runtime_error("invalid escape character " + std::to_string(code));
		}
	}
	// skip through the closing single quote
	ptr++;
	return std::make_shared<Token>(CHAR_LIT, result);
}

token_ptr lexer::Lexer::make_special() {
	// get the token type and value from the specials map (lexer.h)
	token_ptr e = std::make_shared<Token>(specials[src[ptr]], std::to_string(src[ptr]));
	// advance and return
	ptr++;
	return e;
}

void lexer::Lexer::skip_comment() {
	// while the pointer is in the bounds and not encountered newline, advance
	while (src[ptr] != '\n' && ptr < src.size()) {
		ptr++;
	}
	// skip the newline
	ptr++;
}

void lexer::Lexer::skip_multiline_comment() {
	// advance through the ~
	++ptr;
	// while still in bounds
	while (ptr < src.size())
		// if encountered closing comment, break
		if (src[ptr++] == '~') break;
}