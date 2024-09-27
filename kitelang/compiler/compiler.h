#pragma once
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <map>
#include "../parser/parser.h"

namespace compiler {
	typedef struct {
		std::vector<ktypes::ktype_t> argtps;
		ktypes::ktype_t returns;
	} kfndec_t;

	class Compiler {
	private:
		std::map<std::string, std::string> b32r{
			{"rax", "eax"},
			{"rcx", "ecx"},
			{"rdx", "edx"},
			{"rbx", "ebx"},
			{"rsi", "esi"},
			{"rdi", "edi"},
			{"rsp", "esp"},
			{"rbp", "ebp"},
			{"r8", "r8d"},
			{"r9", "r9d"},
			{"r10", "r10d"},
			{"r11", "r11d"},
			{"r12", "r12d"},
			{"r13", "r13d"},
			{"r14", "r14d"},
			{"r15", "r15d"},
		};
		std::map<std::string, std::string> b16r{
			{"rax", "ax"},
			{"rcx", "cx"},
			{"rdx", "dx"},
			{"rbx", "bx"},
			{"rsi", "si"},
			{"rdi", "di"},
			{"rsp", "sp"},
			{"rbp", "bp"},
			{"r8", "r8w"},
			{"r9", "r9w"},
			{"r10", "r10w"},
			{"r11", "r11w"},
			{"r12", "r12w"},
			{"r13", "r13w"},
			{"r14", "r14w"},
			{"r15", "r15w"},
		};
		std::map<std::string, std::string> b8r{
			{"rax", "al"},
			{"rcx", "cl"},
			{"rdx", "dl"},
			{"rbx", "bl"},
			{"rsi", "sil"},
			{"rdi", "dil"},
			{"rsp", "spl"},
			{"rbp", "bpl"},
			{"r8", "r8b"},
			{"r9", "r9b"},
			{"r10", "r10b"},
			{"r11", "r11b"},
			{"r12", "r12b"},
			{"r13", "r13b"},
			{"r14", "r14b"},
			{"r15", "r15b"},
		};
		std::string txbreg(std::string, ktypes::ktype_t);
		std::string argregs[6] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
		std::map<std::string, std::string> cmpkeywordinstruction = {
			{"eq", "je"},
			{"neq", "jne"},
		};
		std::string curFn;							// the current function the compiler is inside
		int curLoopId = 0;							// the current loop ID the compiler is inside
		std::shared_ptr<parser::Node> curLoop;	    // the current loop the compiler is inside
		int cmpLabelCount = 0;
		int dataSectionCount = 0;
		std::string tab = "    ";
		std::shared_ptr<parser::RootNode> root;
		std::vector<std::string> dataSection;
		std::vector<std::string> textSection;
		void visit_node(std::shared_ptr<parser::Node>, std::string = "");
		void visit_root(std::shared_ptr<parser::RootNode>);
		void visit_root_with_scope(std::shared_ptr<parser::RootNode>);
		int visit_root_with_scope_return_amt(std::shared_ptr<parser::RootNode>);
		void visit_int_lit(std::shared_ptr<parser::IntLitNode>, std::string);
		void visit_char_lit(std::shared_ptr<parser::CharLitNode>, std::string);
		void visit_reg(std::shared_ptr<parser::RegNode>, std::string);
		void visit_addrof(std::shared_ptr<parser::AddrOfNode>, std::string);
		void visit_deref(std::shared_ptr<parser::DerefNode>, std::string);
		void visit_var(std::shared_ptr<parser::VarNode>, std::string);
		void visit_idx(std::shared_ptr<parser::IndexNode>, std::string);
		void visit_string_lit(std::shared_ptr<parser::StringLitNode>, std::string);
		void visit_call(std::shared_ptr<parser::CallNode>, std::string);
		void visit_extern(std::shared_ptr<parser::ExternNode>);
		void visit_global(std::shared_ptr<parser::GlobalNode>);
		void visit_fn(std::shared_ptr<parser::FnNode>);
		void visit_return(std::shared_ptr<parser::ReturnNode>);
		void visit_break();
		void visit_continue();
		void visit_cmp(std::shared_ptr<parser::CmpNode>);
		void visit_if(std::shared_ptr<parser::IfNode>);
		void visit_asm(std::shared_ptr<parser::AsmNode>);
		void visit_for(std::shared_ptr<parser::ForNode>);
		void visit_loop(std::shared_ptr<parser::LoopNode>);
		void visit_let(std::shared_ptr<parser::LetNode>);
		void visit_binop(std::shared_ptr<parser::BinOpNode>, std::string);

		void visit_cdirect(std::shared_ptr<parser::CompDirectNode>);

		int get_variable_offset(std::string);

		std::map<std::string, int> varlocs{};
		std::map<std::string, ktypes::ktype_t> vartypes {};
		std::map<std::string, kfndec_t> fns {};
		int stacksize = 0;
		void push(std::string);
		void pop(std::string);
		void pop();
	public:
		Compiler(std::shared_ptr<parser::RootNode> r) : root(r), dataSectionCount(0), curLoopId(0) {}
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