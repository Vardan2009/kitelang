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
	if (peek()->type != lexer::KEYWORD) throw std::runtime_error("expected a statement");
	std::string stmt = peek()->value_str;
	if (stmt == "global") return global_node();
	if (stmt == "extern") return extern_node();
	if (stmt == "call") return call_node();
	if (stmt == "routine") return routine_node();
	throw std::runtime_error("unknown statement " + stmt);
}

std::shared_ptr<parser::Node> parser::Parser::expr() {
	std::shared_ptr<Node> n = term();
	while (peek()->type == lexer::PLUS || peek()->type == lexer::MINUS) {
		std::shared_ptr<lexer::Token> op = advance();
		std::shared_ptr<Node> r = term();
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

std::shared_ptr<parser::CallNode> parser::Parser::call_node() {
	consume(lexer::KEYWORD, "call");
	std::string name = advance()->value_str;
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

std::shared_ptr<parser::RoutineNode> parser::Parser::routine_node() {
	consume(lexer::KEYWORD, "routine");
	std::string name = advance()->value_str;
	std::shared_ptr<RootNode> root = statement_list();
	return std::make_shared<RoutineNode>(name, root);
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