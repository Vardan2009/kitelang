#include "parser.h"

std::shared_ptr<parser::RootNode> parser::Parser::statement_list() {
	if (peek()->type == lexer::LBRACE) advance();
	std::vector<std::shared_ptr<Node>> statements;
	while (ptr < tokens.size() && peek()->type != lexer::RBRACE) {
		statements.push_back(statement());
	}
	if (ptr < tokens.size() && peek()->type == lexer::RBRACE) advance();
	return std::make_shared<RootNode>(statements);
}

std::shared_ptr<parser::Node> parser::Parser::statement() {
	std::string stmt = peek()->value_str;
	if (stmt == "global" && peek()->type == lexer::KEYWORD) return global_node();
	if (stmt == "extern" && peek()->type == lexer::KEYWORD) return extern_node();
	if (stmt == "routine" && peek()->type == lexer::KEYWORD) return routine_node();
	if (stmt == "cmp" && peek()->type == lexer::KEYWORD) return cmp_node();
	if (stmt == "let" && peek()->type == lexer::KEYWORD) return let_node();
	if (peek()->type == lexer::LBRACE) return statement_list();
	return expr();
}

std::shared_ptr<parser::Node> parser::Parser::expr() {
	std::shared_ptr<Node> n = term();
	while (peek()->type == lexer::PLUS || peek()->type == lexer::MINUS) {
		std::shared_ptr<lexer::Token> op = advance();
		std::shared_ptr<Node> r = term();
		n = std::make_shared<BinOpNode>(n, op->type, r);
	}
	if (peek()->type == lexer::EQ) {
		std::shared_ptr<lexer::Token> op = advance();
		std::shared_ptr<Node> r = expr();
		n = std::make_shared<BinOpNode>(n, op->type, r);
	}
	return n;
}


std::shared_ptr<parser::Node> parser::Parser::term() {
	std::shared_ptr<Node> n = factor();
	while (peek()->type == lexer::MUL || peek()->type == lexer::DIV) {
		std::shared_ptr<lexer::Token> op = advance();
		std::shared_ptr<Node> r = factor();
		n = std::make_shared<BinOpNode>(n, op->type, r);
	}
	return n;
}

std::shared_ptr<parser::Node> parser::Parser::factor() {
	if (peek()->type == lexer::INT_LIT)
		return std::make_shared<IntLitNode>(advance()->value);
	if (peek()->type == lexer::STRING_LIT)
		return std::make_shared<StringLitNode>(advance()->value_str);
	if (peek()->type == lexer::LPAREN) {
		consume(lexer::LPAREN);
		std::shared_ptr<parser::Node> n = expr();
		consume(lexer::RPAREN);
		return n;
	}
	if (peek()->type == lexer::IDENTIFIER) {
		std::string name = advance()->value_str;
		if (peek()->type != lexer::LPAREN)
			return std::make_shared<VarNode>(name);
		consume(lexer::LPAREN);
		std::vector<std::shared_ptr<Node>> args;
		while (peek()->type != lexer::RPAREN) {
			args.push_back(expr());
			if (peek()->type != lexer::COMMA) break;
			consume(lexer::COMMA);
		}
		consume(lexer::RPAREN);
		return std::make_shared<CallNode>(name, args);
	}
	throw std::runtime_error("invalid factor " + std::to_string(peek()->type));
}

std::shared_ptr<parser::GlobalNode> parser::Parser::global_node() {
	consume(lexer::KEYWORD, "global");
	return std::make_shared<GlobalNode>(advance()->value_str);
}

std::shared_ptr<parser::ExternNode> parser::Parser::extern_node() {
	consume(lexer::KEYWORD, "extern");
	return std::make_shared<ExternNode>(advance()->value_str);
}


std::shared_ptr<parser::RoutineNode> parser::Parser::routine_node() {
	consume(lexer::KEYWORD, "routine");
	std::string name = advance()->value_str;
	std::shared_ptr<RootNode> root = statement_list();
	return std::make_shared<RoutineNode>(name, root);
}

std::shared_ptr<parser::CmpNode> parser::Parser::cmp_node() {
	consume(lexer::KEYWORD, "cmp");
	std::shared_ptr<Node> val1 = expr();
	consume(lexer::COMMA);
	std::shared_ptr<Node> val2 = expr();
	consume(lexer::LBRACE);
	std::map<std::string, std::shared_ptr<RootNode>> comparisons {};
	while (peek()->type != lexer::RBRACE) {
		if (peek()->type != lexer::KEYWORD)
			throw std::runtime_error("expected a comparison result keyword at cmp block!");
		std::string key = advance()->value_str;
		comparisons[key] = statement_list();
	}
	consume(lexer::RBRACE);
	return std::make_shared<CmpNode>(val1, val2, comparisons);
}

std::shared_ptr<parser::LetNode> parser::Parser::let_node() {
	consume(lexer::KEYWORD, "let");
	std::string name = advance()->value_str;
	consume(lexer::EQ);
	std::shared_ptr<Node> root = expr();
	return std::make_shared<LetNode>(name, root);
}

std::shared_ptr<lexer::Token> parser::Parser::peek() {
	return tokens[ptr];
}

std::shared_ptr<lexer::Token> parser::Parser::advance() {
	return tokens[ptr++];
}

void parser::Parser::consume(lexer::token_t t) {
	if (peek()->type == t) advance();
	else throw std::runtime_error("Unexpected token " + std::to_string(peek()->type) + ", expected: " + std::to_string(t));
}

void parser::Parser::consume(lexer::token_t t, std::string s) {
	if (peek()->type == t && peek()->value_str == s) advance();
	else throw std::runtime_error("Unexpected token " + peek()->value_str + ", expected: " + s);
}