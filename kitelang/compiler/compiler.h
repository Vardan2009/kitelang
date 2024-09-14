#pragma once
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <map>
#include "../parser/parser.h"

namespace compiler {
	class Compiler {
	private:
		std::string argregs[6] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
		std::map<std::string, std::string> cmpkeywordinstruction = {
			{"eq", "je"},
			{"neq", "jne"},
		};
		std::string curFn; // the current function the compiler is inside
		int cmpLabelCount = 0;
		int dataSectionCount = 0;
		std::string tab = "    ";
		std::shared_ptr<parser::RootNode> root;
		std::vector<std::string> dataSection;
		std::vector<std::string> textSection;
		void visit_node(std::shared_ptr<parser::Node>, std::string = "");
		void visit_root(std::shared_ptr<parser::RootNode>);
		void visit_root_with_scope(std::shared_ptr<parser::RootNode>);
		void visit_int_lit(std::shared_ptr<parser::IntLitNode>, std::string);
		void visit_char_lit(std::shared_ptr<parser::CharLitNode>, std::string);
		void visit_var(std::shared_ptr<parser::VarNode>, std::string);
		void visit_string_lit(std::shared_ptr<parser::StringLitNode>, std::string);
		void visit_call(std::shared_ptr<parser::CallNode>, std::string);
		void visit_extern(std::shared_ptr<parser::ExternNode>);
		void visit_global(std::shared_ptr<parser::GlobalNode>);
		void visit_fn(std::shared_ptr<parser::FnNode>);
		void visit_return(std::shared_ptr<parser::ReturnNode>);
		void visit_cmp(std::shared_ptr<parser::CmpNode>);
		void visit_asm(std::shared_ptr<parser::AsmNode>);
		void visit_for(std::shared_ptr<parser::ForNode>);
		void visit_let(std::shared_ptr<parser::LetNode>);
		void visit_binop(std::shared_ptr<parser::BinOpNode>, std::string);

		int get_variable_offset(std::string);

		std::map<std::string, int> vars {};
		int stacksize = 0;
		void push(std::string);
		void pop(std::string);
		void pop();
	public:
		Compiler(std::shared_ptr<parser::RootNode> r) : root(r), dataSectionCount(0) {}
		void codegen();
		void print(std::ostream& stream) {
			stream << "section .data" << std::endl;
			for (std::string instruction : dataSection) {
				stream << tab << instruction << std::endl;
			}
			stream << "section .text" << std::endl;
			for (std::string instruction : textSection) {
				stream << tab << instruction << std::endl;
			}
		}
	};
}