#include "lexer.h"
#include <iostream>

std::vector<token_ptr> lexer::Lexer::tokenize() {
	std::vector<token_ptr> result;
	while (ptr < src.size()) {
		if (isalpha(src[ptr]) || src[ptr] == '_')
			result.push_back(make_identifier());
		else if (isdigit(src[ptr]))
			result.push_back(make_int());
		else if (src[ptr] == '"')
			result.push_back(make_string());
		else if (specials.count(src[ptr]))
			result.push_back(make_special());
		else if (src[ptr] == '%') skip_comment();
		else if (src[ptr] == '~') skip_multiline_comment();
		else if (isspace(src[ptr])) ++ptr;
		else
			throw std::runtime_error("invalid character `" + std::to_string(src[ptr]) + "`");
	}
	return result;
}

token_ptr lexer::Lexer::make_identifier() {
	std::string result;
	while (isalnum(src[ptr]) || src[ptr] == '_') {
		result += src[ptr++];
	}

	token_t type =
		(keywords.find(result) != keywords.end())
		? KEYWORD
		: IDENTIFIER;

	return std::make_shared<Token>(type, result);
}

token_ptr lexer::Lexer::make_int() {
	std::string result;
	while (isdigit(src[ptr])) {
		result += src[ptr++];
	}
	return std::make_shared<Token>(INT_LIT, std::atoi(result.c_str()));
}

token_ptr lexer::Lexer::make_string() {
	ptr++;
	std::string result;
	while (src[ptr] != '"') {
		result += src[ptr++];
	}
	ptr++;
	return std::make_shared<Token>(STRING_LIT, result);
}

token_ptr lexer::Lexer::make_special() {
	token_ptr e = std::make_shared<Token>(specials[src[ptr]], std::to_string(src[ptr]));
	ptr++;
	return e;
}

void lexer::Lexer::skip_comment() {
	while (src[ptr] != '\n' && ptr < src.size()) {
		ptr++;
	}
	ptr++;
}

void lexer::Lexer::skip_multiline_comment() {
	++ptr;
	while (ptr < src.size())
		if (src[ptr++] == '~') break;
}