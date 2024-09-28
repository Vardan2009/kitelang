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
	if (stmt == "fn" && peek()->type == lexer::KEYWORD) return fn_node();
	if (stmt == "return" && peek()->type == lexer::KEYWORD) return return_node();
	if (stmt == "cmp" && peek()->type == lexer::KEYWORD) return cmp_node();
	if (stmt == "if" && peek()->type == lexer::KEYWORD) return if_node();
	if (stmt == "let" && peek()->type == lexer::KEYWORD) return let_node();
	if (stmt == "asm" && peek()->type == lexer::KEYWORD) return asm_node();
	if (stmt == "for" && peek()->type == lexer::KEYWORD) return for_node();
	if (stmt == "loop" && peek()->type == lexer::KEYWORD) return loop_node();
	if (stmt == "break" && peek()->type == lexer::KEYWORD) { advance(); return std::make_shared<BreakNode>();  };
	if (stmt == "continue" && peek()->type == lexer::KEYWORD) { advance(); return std::make_shared<ContinueNode>(); };
	if (peek()->type == lexer::LBRACE) return statement_list();
	if (peek()->type == lexer::CDIRECT) return comp_direct();
	return expr();
}

std::shared_ptr<parser::CompDirectNode> parser::Parser::comp_direct() {
	std::string name = advance()->value_str;
	return std::make_shared<CompDirectNode>(name, advance()->value);
}

std::shared_ptr<parser::Node> parser::Parser::expr() {
	std::shared_ptr<Node> n = term();

	while (true) {
		auto current_token_type = peek()->type;

		if (current_token_type == lexer::PLUS || current_token_type == lexer::MINUS ||
			current_token_type == lexer::EQEQ || current_token_type == lexer::NEQEQ ||
			current_token_type == lexer::GT || current_token_type == lexer::LT ||
			current_token_type == lexer::GTE || current_token_type == lexer::LTE) {

			std::shared_ptr<lexer::Token> op = advance();
			std::shared_ptr<Node> r = term();
			n = std::make_shared<BinOpNode>(n, op->type, r);
		}
		else if (current_token_type == lexer::EQ) {
			std::shared_ptr<lexer::Token> op = advance();
			std::shared_ptr<Node> r = expr();
			n = std::make_shared<BinOpNode>(n, op->type, r);
		}
		else break;
	}

	return n;
}

std::shared_ptr<parser::Node> parser::Parser::term() {
	std::shared_ptr<Node> n = factor();
	while (peek()->type == lexer::MUL || peek()->type == lexer::DIV || peek()->type == lexer::MOD) {
		std::shared_ptr<lexer::Token> op = advance();
		std::shared_ptr<Node> r = factor();
		n = std::make_shared<BinOpNode>(n, op->type, r);
	}
	return n;
}

std::shared_ptr<parser::Node> parser::Parser::factor() {
	switch (peek()->type) {
	case lexer::INT_LIT:
		return std::make_shared<IntLitNode>(advance()->value);
	case lexer::STRING_LIT:
		return std::make_shared<StringLitNode>(advance()->value_str);
	case lexer::CHAR_LIT:
		return std::make_shared<CharLitNode>((char)advance()->value);
	case lexer::REG:
		return std::make_shared<RegNode>(advance()->value_str);
	case lexer::ADDROF:
		return std::make_shared<AddrOfNode>(advance()->value_str);
	case lexer::DEREF:
		return std::make_shared<DerefNode>(advance()->value_str);
	case lexer::LPAREN:
		{
			consume(lexer::LPAREN);
			std::shared_ptr<parser::Node> n = expr();
			consume(lexer::RPAREN);
			return n;
		}
	case lexer::IDENTIFIER:
		{
			std::string name = advance()->value_str;
			if (peek()->type == lexer::LSQR) {
				consume(lexer::LSQR);
				std::shared_ptr<Node> index = expr();
				consume(lexer::RSQR);
				return std::make_shared<IndexNode>(name, index);
			}
			else if (peek()->type == lexer::LPAREN) {
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
			else
				return std::make_shared<VarNode>(name);
		}
	default:
		throw std::runtime_error("invalid factor " + std::to_string(peek()->type));
	}
}

std::shared_ptr<parser::GlobalNode> parser::Parser::global_node() {
	consume(lexer::KEYWORD, "global");
	if (peek()->type == lexer::LBRACE) {
		std::vector<std::string> symbols{};
		consume(lexer::LBRACE);
		while (peek()->type != lexer::RBRACE) {
			if (peek()->type != lexer::IDENTIFIER)
				throw std::runtime_error("expected identifier");
			symbols.push_back(advance()->value_str);
			if (peek()->type == lexer::RBRACE) break;
			consume(lexer::COMMA);
		}
		consume(lexer::RBRACE);
		return std::make_shared<GlobalNode>(symbols);
	}
	if (peek()->type != lexer::IDENTIFIER)
		throw std::runtime_error("expected identifier");
	return std::make_shared<GlobalNode>(std::vector<std::string>{ advance()->value_str });
}

std::shared_ptr<parser::ExternNode> parser::Parser::extern_node() {
	consume(lexer::KEYWORD, "extern");
	if (peek()->type == lexer::LBRACE) {
		std::vector<ktypes::kfndec_t> fns {};
		consume(lexer::LBRACE);
		while (peek()->type != lexer::RBRACE) {
			if (peek()->type != lexer::IDENTIFIER)
				throw std::runtime_error("expected identifier");
			std::string name = advance()->value_str;
			std::vector<ktypes::ktype_t> types{};
			consume(lexer::LPAREN);
			while (peek()->type != lexer::RPAREN) {
				types.push_back(type());
				if (peek()->type == lexer::RPAREN) break;
				consume(lexer::COMMA);
			}
			consume(lexer::RPAREN);
			consume(lexer::COLON);
			ktypes::ktype_t returns = type();
			fns.push_back(ktypes::kfndec_t{name, types, returns});
			if (peek()->type == lexer::RBRACE) break;
			consume(lexer::COMMA);
		}
		consume(lexer::RBRACE);
		return std::make_shared<ExternNode>(fns);
	}
	if (peek()->type != lexer::IDENTIFIER)
		throw std::runtime_error("expected identifier");
	std::string name = advance()->value_str;
	std::vector<ktypes::ktype_t> types;
	consume(lexer::LPAREN);
	while (peek()->type != lexer::RPAREN) {
		types.push_back(type());
		if (peek()->type == lexer::RPAREN) break;
		consume(lexer::COMMA);
	}
	consume(lexer::RPAREN);
	consume(lexer::COLON);
	ktypes::ktype_t returns = type();
	return std::make_shared<ExternNode>(std::vector<ktypes::kfndec_t>{ ktypes::kfndec_t{ name, types, returns } });
}

std::shared_ptr<parser::ReturnNode> parser::Parser::return_node() {
	consume(lexer::KEYWORD, "return");
	return std::make_shared<ReturnNode>(expr());
}

std::shared_ptr<parser::FnNode> parser::Parser::fn_node() {
	consume(lexer::KEYWORD, "fn");
	std::string name = advance()->value_str;
	std::vector <ktypes::kval_t> args {};
	consume(lexer::LPAREN);
	while (peek()->type != lexer::RPAREN) {
		if (peek()->type != lexer::IDENTIFIER)
			throw std::runtime_error("expected an identifier");
		std::string argnm = advance()->value_str;
		consume(lexer::COLON);
		ktypes::ktype_t argtp = type();
		args.push_back(ktypes::kval_t{argnm, argtp});
		if (peek()->type == lexer::RPAREN) break;
		consume(lexer::COMMA);
	}
	consume(lexer::RPAREN);
	consume(lexer::COLON);
	ktypes::ktype_t returns = type();
	std::shared_ptr<RootNode> root = statement_list();
	return std::make_shared<FnNode>(name, args, returns, root);
}

std::shared_ptr<parser::IfNode> parser::Parser::if_node() {
	consume(lexer::KEYWORD, "if");
	std::shared_ptr<Node> condition = expr();
	std::shared_ptr<Node> block = statement();
	return std::make_shared<IfNode>(condition, block);
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

std::shared_ptr<parser::AsmNode> parser::Parser::asm_node() {
	consume(lexer::KEYWORD, "asm");
	return std::make_shared<AsmNode>(advance()->value_str);
}

std::shared_ptr<parser::LoopNode> parser::Parser::loop_node() {
	consume(lexer::KEYWORD, "loop");
	return std::make_shared<LoopNode>(statement());
}

std::shared_ptr<parser::ForNode> parser::Parser::for_node() {
	consume(lexer::KEYWORD, "for");
	std::string itername = advance()->value_str;
	consume(lexer::EQ);
	std::shared_ptr<Node> initVal = expr();
	consume(lexer::ARROW);
	std::shared_ptr<Node> targetVal = expr();
	consume(lexer::CARET);
	std::shared_ptr<Node> stepVal = expr();
	std::shared_ptr<Node> root = statement();
	return std::make_shared<ForNode>(itername, root, initVal, targetVal, stepVal);
}

std::shared_ptr<parser::LetNode> parser::Parser::let_node() {
	consume(lexer::KEYWORD, "let");
	std::string name = advance()->value_str;
	consume(lexer::COLON);
	ktypes::ktype_t tp = type();
	if (peek()->type == lexer::EQ) {
		consume(lexer::EQ);
		std::shared_ptr<Node> root = expr();
		return std::make_shared<LetNode>(name, tp, root);
	}
	else if (peek()->type == lexer::LSQR) {
		consume(lexer::LSQR);
		if (peek()->type != lexer::INT_LIT)
			throw std::runtime_error("allocation size should be an integer literal");
		int allocVal = advance()->value;
		consume(lexer::RSQR);
		return std::make_shared<LetNode>(name, tp, allocVal);
	}
	else throw std::runtime_error("expected [ or = after let");
}

ktypes::ktype_t parser::Parser::type() {
	if (peek()->type != lexer::KEYWORD || !ktypes::nktype_t.contains(peek()->value_str))
		throw std::runtime_error("expected type specifier");
	return ktypes::from_string(advance()->value_str);
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