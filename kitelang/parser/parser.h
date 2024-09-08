#pragma once
#include "node.h"

namespace parser {
	class Parser {
	private:
		std::vector<std::shared_ptr<lexer::Token>> tokens;

		std::shared_ptr<RootNode> statement_list();
		std::shared_ptr<Node> statement();
		std::shared_ptr<Node> expr();
		std::shared_ptr<Node> term();
		std::shared_ptr<Node> factor();

		std::shared_ptr<GlobalNode> global_node();
		std::shared_ptr<ExternNode> extern_node();
		std::shared_ptr<RoutineNode> routine_node();
		std::shared_ptr<CmpNode> cmp_node();
		std::shared_ptr<AsmNode> asm_node();
		std::shared_ptr<LetNode> let_node();

		std::shared_ptr<lexer::Token> peek();
		std::shared_ptr<lexer::Token> advance();
		void consume(lexer::token_t);
		void consume(lexer::token_t, std::string);

	public:
		int ptr = 0;
		Parser(std::vector<std::shared_ptr<lexer::Token>> t) : tokens(t), ptr(0) {
		}
		std::shared_ptr<RootNode> parse() {
			return statement_list();
		}
	};
}