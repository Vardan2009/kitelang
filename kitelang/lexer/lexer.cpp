#include "lexer.h"

std::vector<token_ptr> lexer::Lexer::tokenize() {
	// initialize the array (vector)
	std::vector<token_ptr> result {};
	// Initialize line and position counters
	this->line = 1;
	this->pos = 1;

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
		// if it is an identifier with prefix
		else if (prefixes.count(src[ptr]) && isalpha(src[(size_t)(ptr + 1)]))
			result.push_back(make_with_prefix(prefixes[src[ptr]]));
		// then it is a special token (with two characters)
		else if ((ptr < src.size() - 1) && (specialsTwoChar.count(std::string(1, src[ptr]) + src[(size_t)(ptr + 1)])))
			result.push_back(make_special_two());
		else if (specials.count(src[ptr]))
			result.push_back(make_special());
		// if it is a single line comment prefix, skip the comment
		else if (src[ptr] == ';') skip_comment();
		// if it is a multiline comment prefix, skip the comment
		else if (src[ptr] == '~') skip_multiline_comment();
		// ignore whitespace
		else if (isspace(src[ptr])) advance();
		// case for invalid characters
		else
			throw errors::kiterr("invalid character `" + std::to_string(src[ptr]) + "`", this->line, this->pos, this->pos);
	}
	return result;
}

token_ptr lexer::Lexer::make_identifier() {
	// this will store the result
	std::string result;
	int pos_start = this->pos;

	// while the current character is alphanumeric or an underscore
	while (isalnum(src[ptr]) || src[ptr] == '_') {
		// add to the result and increment the pointer
		result += advance();
	}

	// determine if the "word" is an identifier or a keyword
	// if the result is in the "keywords" list (in lexer.h), then it is a keyword
	token_t type =
		(keywords.find(result) != keywords.end())
		? KEYWORD
		: IDENTIFIER;

	return std::make_shared<Token>(type, result, this->line, pos_start, this->pos);
}

token_ptr lexer::Lexer::make_int() {
	// this will store the result (as a string for now)
	std::string result;
	int pos_start = this->pos;

	// while it is a digit, add to the result and increment the pointer
	while (isdigit(src[ptr])) {
		result += advance();
	}

	// convert the result to integer (atoi) and return
	return std::make_shared<Token>(INT_LIT, std::atoi(result.c_str()), this->line, pos_start, this->pos);
}

token_ptr lexer::Lexer::make_string() {
	int pos_start = this->pos;
	int line_start = this->line;
	// skip through the double quote
	advance();

	// this will store the result
	std::string result;

	// while the current character is not a closing quote
	while (src[ptr] != '"') {
		if (src[ptr] == '\\')
			result += advance();
		result += advance();
	}

	// skip through the closing double quote
	advance();
	return std::make_shared<Token>(STRING_LIT, result, line_start, pos_start, pos_start);
}

token_ptr lexer::Lexer::make_with_prefix(token_t type) {
	int pos_start = this->pos;

	// skip through the prefix
	advance();

	// this will store the result
	std::string result;

	// while the current character is alphanumeric or an underscore
	while (isalnum(src[ptr])) {
		// add to the result and increment the pointer
		result += advance();
	}

	return std::make_shared<Token>(type, result, this->line, pos_start, this->pos);
}

token_ptr lexer::Lexer::make_char() {
	int pos_start = this->pos;

	// skip through the single quote
	advance();
	// get the result and increment, since it is guaranteed to be a single character
	char result = advance();
	if (result == '\\') {
		char code = advance();
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
			throw errors::kiterr("invalid escape character " + std::to_string(code), this->line, this->pos, this->pos);
		}
	}
	// skip through the closing single quote
	advance();
	return std::make_shared<Token>(CHAR_LIT, result, this->line, pos_start, this->pos);
}

token_ptr lexer::Lexer::make_special() {
	// get the token type and value from the specials map (lexer.h)
	token_ptr e = std::make_shared<Token>(specials[src[ptr]], std::to_string(src[ptr]), this->line, this->pos, this->pos);
	// advance and return
	advance();
	return e;
}

token_ptr lexer::Lexer::make_special_two() {
	// get the token type and value from the specialsTwoChar map (lexer.h)
	std::string key(1, src[ptr]);
	key += src[(size_t)(ptr + 1)];
	token_ptr e = std::make_shared<Token>(
		specialsTwoChar[
			key
		],
		key,
		this->line,
		this->pos,
		this->pos + 1
	);
	// advance and return
	advance();
	advance();
	return e;
}

void lexer::Lexer::skip_comment() {
	// while the pointer is in the bounds and not encountered newline, advance
	while (src[ptr] != '\n' && ptr < src.size()) advance();
	// skip the newline
	advance();
}

void lexer::Lexer::skip_multiline_comment() {
	// advance through the ~
	advance();
	// while still in bounds
	while (ptr < src.size())
		// if encountered closing comment, break
		if (advance() == '~') break;
}

char lexer::Lexer::advance() {
	// if it is a line break
	if (src[ptr] == '\n') {
		// reset position counter and
		// increment line counter
		pos = 1;
		++line;
	}
	else ++pos;
	return src[ptr++];
}