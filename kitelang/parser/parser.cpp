#include "parser.h"

std::shared_ptr<parser::RootNode> parser::Parser::statement_list(bool isroot) {
	if (peek()->type != lexer::LBRACE && !isroot)
		throw errors::kiterr("expected {", peek()->line, peek()->pos_start, peek()->pos_end);
	std::shared_ptr<lexer::Token> t = isroot ? peek() : advance();
	int line = t->line, pos = t->pos_start;
	
	std::vector<std::shared_ptr<Node>> statements;
	while (ptr < tokens.size() && peek()->type != lexer::RBRACE) {
		statements.push_back(statement());
	}
	if (ptr < tokens.size() && peek()->type == lexer::RBRACE) advance();
	return std::make_shared<RootNode>(statements, line, pos, pos);
}

std::shared_ptr<parser::Node> parser::Parser::statement() {
	std::string stmt = peek()->value_str;
	std::shared_ptr<lexer::Token> t = peek();
	if (stmt == "global" && t->type == lexer::KEYWORD) return global_node();
	if (stmt == "extern" && t->type == lexer::KEYWORD) return extern_node();
	if (stmt == "fn" && t->type == lexer::KEYWORD) return fn_node();
	if (stmt == "return" && t->type == lexer::KEYWORD) return return_node();
	if (stmt == "cmp" && t->type == lexer::KEYWORD) return cmp_node();
	if (stmt == "if" && t->type == lexer::KEYWORD) return if_node();
	if (stmt == "let" && t->type == lexer::KEYWORD) return let_node();
	if (stmt == "asm" && t->type == lexer::KEYWORD) return asm_node();
	if (stmt == "for" && t->type == lexer::KEYWORD) return for_node();
	if (stmt == "loop" && t->type == lexer::KEYWORD) return loop_node();
	if (stmt == "break" && t->type == lexer::KEYWORD) { advance();  return std::make_shared<BreakNode>(t->line, t->pos_start, t->pos_end); };
	if (stmt == "continue" && t->type == lexer::KEYWORD) { advance(); return std::make_shared<ContinueNode>(t->line, t->pos_start, t->pos_end); };
	if (t->type == lexer::LBRACE) return statement_list();
	if (t->type == lexer::CDIRECT) return comp_direct();
	return expr();
}

std::shared_ptr<parser::CompDirectNode> parser::Parser::comp_direct() {
	std::shared_ptr<lexer::Token> t = advance();
	return std::make_shared<CompDirectNode>(t->value_str, advance()->value, t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::Node> parser::Parser::expr() {
	std::shared_ptr<Node> n = term();
	int line = n->line;
	int pos_start = n->pos_start;

	while (true) {
		auto current_token_type = peek()->type;

		if (current_token_type == lexer::PLUS || current_token_type == lexer::MINUS ||
			current_token_type == lexer::EQEQ || current_token_type == lexer::NEQEQ ||
			current_token_type == lexer::GT || current_token_type == lexer::LT ||
			current_token_type == lexer::GTE || current_token_type == lexer::LTE) {

			std::shared_ptr<lexer::Token> op = advance();
			std::shared_ptr<Node> r = term();
			n = std::make_shared<BinOpNode>(n, op->type, r, line, pos_start, n->pos_end);
		}
		else if (current_token_type == lexer::EQ) {
			std::shared_ptr<lexer::Token> op = advance();
			std::shared_ptr<Node> r = expr();
			n = std::make_shared<BinOpNode>(n, op->type, r, line, pos_start, n->pos_end);
		}
		else break;
	}

	return n;
}

std::shared_ptr<parser::Node> parser::Parser::term() {
	std::shared_ptr<Node> n = factor();
	int line = n->line;
	int pos_start = n->pos_start;

	while (peek()->type == lexer::MUL || peek()->type == lexer::DIV || peek()->type == lexer::MOD) {
		std::shared_ptr<lexer::Token> op = advance();
		std::shared_ptr<Node> r = factor();
		n = std::make_shared<BinOpNode>(n, op->type, r, line, pos_start, n->pos_end);
	}
	return n;
}

std::shared_ptr<parser::Node> parser::Parser::factor() {
	std::shared_ptr<lexer::Token> t = advance();
	switch (t->type) {
	case lexer::INT_LIT:
		return std::make_shared<IntLitNode>(t->value, t->line, t->pos_start, t->pos_end);
	case lexer::STRING_LIT:
		return std::make_shared<StringLitNode>(t->value_str, t->line, t->pos_start, t->pos_end);
	case lexer::CHAR_LIT:
		return std::make_shared<CharLitNode>((char)t->value, t->line, t->pos_start);
	case lexer::REG:
		return std::make_shared<RegNode>(t->value_str, t->line, t->pos_start, t->pos_end);
	case lexer::ADDROF:
		return std::make_shared<AddrOfNode>(t->value_str, t->line, t->pos_start, t->pos_end);
	case lexer::DEREF:
		return std::make_shared<DerefNode>(t->value_str, t->line, t->pos_start, t->pos_end);
	case lexer::LPAREN:
		{
			std::shared_ptr<parser::Node> n = expr();
			consume(lexer::RPAREN);
			return n;
		}
	case lexer::IDENTIFIER:
		{
			std::string name = t->value_str;
			if (peek()->type == lexer::LSQR) {
				consume(lexer::LSQR);
				std::shared_ptr<Node> index = expr();
				consume(lexer::RSQR);
				return std::make_shared<IndexNode>(name, index, t->line, t->pos_start, t->pos_end);
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
				return std::make_shared<CallNode>(name, args, t->line, t->pos_start, t->pos_end);
			}
			else
				return std::make_shared<VarNode>(name, t->line, t->pos_start, t->pos_end);
		}
	default:
		throw errors::kiterr("invalid factor " + std::to_string(peek()->type), t->line, t->pos_start, t->pos_end);
	}
}

std::shared_ptr<parser::GlobalNode> parser::Parser::global_node() {
	std::shared_ptr<lexer::Token> t = advance();
	if (peek()->type == lexer::LBRACE) {
		std::vector<std::string> symbols{};
		consume(lexer::LBRACE);
		while (peek()->type != lexer::RBRACE) {
			if (peek()->type != lexer::IDENTIFIER)
				throw errors::kiterr("expected identifier", peek()->line, peek()->pos_start, peek()->pos_end);
			symbols.push_back(advance()->value_str);
			if (peek()->type == lexer::RBRACE) break;
			consume(lexer::COMMA);
		}
		consume(lexer::RBRACE);
		return std::make_shared<GlobalNode>(symbols, t->line, t->pos_start, t->pos_end);
	}
	if (peek()->type != lexer::IDENTIFIER)
		throw errors::kiterr("expected identifier", peek()->line, peek()->pos_start, peek()->pos_end);
	return std::make_shared<GlobalNode>(std::vector<std::string>{ advance()->value_str }, t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::ExternNode> parser::Parser::extern_node() {
	std::shared_ptr<lexer::Token> t = advance();
	if (peek()->type == lexer::LBRACE) {
		std::vector<ktypes::kfndec_t> fns {};
		consume(lexer::LBRACE);

		while (peek()->type != lexer::RBRACE) {
			if (peek()->type != lexer::IDENTIFIER)
				throw errors::kiterr("expected identifier", peek()->line, peek()->pos_start, peek()->pos_end);
			std::string name = advance()->value_str;
			std::vector<ktypes::ktype_t> types{};
			consume(lexer::LPAREN);
			bool is_variadic = false;
			while (peek()->type != lexer::RPAREN) {
				if (peek()->type == lexer::VAARG) {
					advance();
					is_variadic = true;
					break;
				}
				types.push_back(type());
				if (peek()->type == lexer::RPAREN) break;
				consume(lexer::COMMA);
			}
			consume(lexer::RPAREN);
			consume(lexer::COLON);
			ktypes::ktype_t returns = type();
			fns.push_back(ktypes::kfndec_t{name, types, returns, is_variadic});
			if (peek()->type == lexer::RBRACE) break;
			consume(lexer::COMMA);
		}
		consume(lexer::RBRACE);
		return std::make_shared<ExternNode>(fns, t->line, t->pos_start, t->pos_end);
	}
	if (peek()->type != lexer::IDENTIFIER)
		throw errors::kiterr("expected identifier", peek()->line, peek()->pos_start, peek()->pos_end);
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
	return std::make_shared<ExternNode>(std::vector<ktypes::kfndec_t>{ ktypes::kfndec_t{ name, types, returns } }, t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::ReturnNode> parser::Parser::return_node() {
	std::shared_ptr<lexer::Token> t = advance();
	return std::make_shared<ReturnNode>(expr(), t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::FnNode> parser::Parser::fn_node() {
	std::shared_ptr<lexer::Token> t = advance();
	std::string name = advance()->value_str;
	std::vector <ktypes::kval_t> args {};
	bool is_variadic = false;
	consume(lexer::LPAREN);
	while (peek()->type != lexer::RPAREN) {
		if (peek()->type == lexer::VAARG) {
			advance();
			is_variadic = true;
			break;
		}
		if (peek()->type != lexer::IDENTIFIER)
			throw errors::kiterr("expected identifier", peek()->line, peek()->pos_start, peek()->pos_end);
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
	return std::make_shared<FnNode>(name, args, returns, root, is_variadic, t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::IfNode> parser::Parser::if_node() {
	std::shared_ptr<lexer::Token> t = advance();
	std::shared_ptr<Node> condition = expr();
	std::shared_ptr<Node> block = statement();
	std::shared_ptr<IfNode> ifn = std::make_shared<IfNode>(condition, block, t->line, t->pos_start, t->pos_end);
	if (peek()->type == lexer::KEYWORD && peek()->value_str == "else") {
		consume(lexer::KEYWORD, "else");
		ifn->else_block = statement();
		ifn->has_else_block = true;
	}
	else {
		ifn->else_block = nullptr;
		ifn->has_else_block = false;
	}
	return ifn;
}

std::shared_ptr<parser::CmpNode> parser::Parser::cmp_node() {
	std::shared_ptr<lexer::Token> t = advance();
	std::shared_ptr<Node> val1 = expr();
	consume(lexer::COMMA);
	std::shared_ptr<Node> val2 = expr();
	consume(lexer::LBRACE);
	std::map<std::string, std::shared_ptr<RootNode>> comparisons {};
	while (peek()->type != lexer::RBRACE) {
		if (peek()->type != lexer::KEYWORD)
			throw errors::kiterr("expected comparison (eq, neq,...)", peek()->line, peek()->pos_start, peek()->pos_end);
		std::string key = advance()->value_str;
		comparisons[key] = statement_list();
	}
	consume(lexer::RBRACE);
	return std::make_shared<CmpNode>(val1, val2, comparisons, t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::AsmNode> parser::Parser::asm_node() {
	std::shared_ptr<lexer::Token> t = advance();
	return std::make_shared<AsmNode>(advance()->value_str, t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::LoopNode> parser::Parser::loop_node() {
	std::shared_ptr<lexer::Token> t = advance();
	return std::make_shared<LoopNode>(statement(), t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::ForNode> parser::Parser::for_node() {
	std::shared_ptr<lexer::Token> t = advance();
	std::string itername = advance()->value_str;
	consume(lexer::EQ);
	std::shared_ptr<Node> initVal = expr();
	consume(lexer::ARROW);
	std::shared_ptr<Node> targetVal = expr();
	consume(lexer::CARET);
	std::shared_ptr<Node> stepVal = expr();
	std::shared_ptr<Node> root = statement();
	return std::make_shared<ForNode>(itername, root, initVal, targetVal, stepVal, t->line, t->pos_start, t->pos_end);
}

std::shared_ptr<parser::LetNode> parser::Parser::let_node() {
	std::shared_ptr<lexer::Token> t = advance();
	std::string name = advance()->value_str;
	consume(lexer::COLON);
	ktypes::ktype_t tp = type();
	if (peek()->type == lexer::EQ) {
		consume(lexer::EQ);
		std::shared_ptr<Node> root = expr();
		return std::make_shared<LetNode>(name, tp, root, t->line, t->pos_start, t->pos_end);
	}
	else if (peek()->type == lexer::LSQR) {
		consume(lexer::LSQR);
		if (peek()->type != lexer::INT_LIT)
			throw errors::kiterr("allocation size should be an integer literal", peek()->line, peek()->pos_start, peek()->pos_end);
		int allocVal = advance()->value;
		consume(lexer::RSQR);
		return std::make_shared<LetNode>(name, tp, allocVal, t->line, t->pos_start, t->pos_end);
	}
	else throw errors::kiterr("expected = or [", peek()->line, peek()->pos_start, peek()->pos_end);
}

ktypes::ktype_t parser::Parser::type() {
	if (peek()->type != lexer::KEYWORD || !ktypes::nktype_t.contains(peek()->value_str))
		throw errors::kiterr("expected type specifier", peek()->line, peek()->pos_start, peek()->pos_end);
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
	else throw errors::kiterr("Unexpected token " + std::to_string(peek()->type) + ", expected: " + std::to_string(t), peek()->line, peek()->pos_start, peek()->pos_end);
}

void parser::Parser::consume(lexer::token_t t, std::string s) {
	if (peek()->type == t && peek()->value_str == s) advance();
	else throw errors::kiterr("Unexpected token " + peek()->value_str + ", expected: " + s, peek()->line, peek()->pos_start, peek()->pos_end);
}