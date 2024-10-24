#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <map>

#include "../common.h"
#include "../lexer/token.h"

namespace parser {
	typedef enum {
		NONE,
		ROOT,
		BINOP,
		STRING_LIT,
		INT_LIT,
		CHAR_LIT,
		REG,
		ADDROF,
		DEREF,
		EXTERN,
		GLOBAL,
		FN,
		RETURN,
		BREAK,
		CONTINUE,
		CALL,
		LET,
		IDX,
		VAR,
		CMP,
		IF,
		ASM,
		FOR,
		LOOP,
		CDIRECT
	} node_t;
	class Node {
	public:
		node_t type = NONE;
		int line;
		int pos_start;
		int pos_end;
		virtual void print(int) const = 0;
	};
	class RootNode : public Node {
	public:
		std::vector<std::shared_ptr<Node>> statements;
		RootNode(std::vector<std::shared_ptr<Node>> stmts, int line, int pos_start, int pos_end)
			: statements(stmts) {
			type = ROOT;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "root" << std::endl;
			for (std::shared_ptr<Node> st : statements) {
				st->print(indent + 1);
			}
		}
	};
	class BinOpNode : public Node {
	public:
		std::shared_ptr<Node> left, right;
		lexer::token_t operation;
		BinOpNode(std::shared_ptr<Node> l, lexer::token_t op, std::shared_ptr<Node> r, int line, int pos_start, int pos_end)
			: left(l), right(r), operation(op) {
			type = BINOP;
			line = left->line;
			pos_start = left->pos_start;
			pos_end = right->pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << operation << std::endl;
			left->print(indent + 1);
			right->print(indent + 1);
		}
	};
	class CharLitNode : public Node {
	public:
		char value;
		CharLitNode(char val, int line, int pos)
			: value(val) {
			type = CHAR_LIT;
			this->line = line;
			pos_start = pos_end = pos;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << value << std::endl;
		}
	};
	class RegNode : public Node {
	public:
		std::string value;
		RegNode(std::string val, int line, int pos_start, int pos_end)
			: value(val) {
			type = REG;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "%" << value << std::endl;
		}
	};
	class StringLitNode : public Node {
	public:
		std::string value;
		StringLitNode(std::string val, int line, int pos_start, int pos_end)
			: value(val) {
			type = STRING_LIT;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << value << std::endl;
		}
	};
	class IntLitNode : public Node {
	public:
		int value;
		IntLitNode(int val, int line, int pos_start, int pos_end)
			: value(val) {
			type = INT_LIT;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << value << std::endl;
		}
	};
	class ExternNode : public Node {
	public:
		std::vector<ktypes::kfndec_t> symbols;
		ExternNode(std::vector<ktypes::kfndec_t> rout, int line, int pos_start, int pos_end)
			: symbols(rout) {
			type = EXTERN;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "extern" << std::endl;
			for (ktypes::kfndec_t s : symbols) {
				for (int i = 0; i < indent + 1; i++) std::cout << "--"; std::cout << ' ';
				std::cout << s.name << std::endl;
			}
		}
	};
	class GlobalNode : public Node {
	public:
		std::vector<std::string> symbols;
		GlobalNode(std::vector<std::string> rout, int line, int pos_start, int pos_end)
			: symbols(rout) {
			type = GLOBAL;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "global" << std::endl;
			for (std::string s : symbols) {
				for (int i = 0; i < indent + 1; i++) std::cout << "--"; std::cout << ' ';
				std::cout << s << std::endl;
			}
		}
	};
	class CallNode : public Node {
	public:
		std::string routine;
		std::vector<std::shared_ptr<Node>> args;
		CallNode(std::string rout, std::vector<std::shared_ptr<Node>> a, int line, int pos_start, int pos_end)
			: routine(rout), args(a) {
			type = CALL;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "call " << routine << std::endl;
			for (std::shared_ptr<Node> st : args) {
				st->print(indent + 1);
			}
		}
	};
	class FnNode : public Node {
	public:
		std::string name;
		std::shared_ptr<RootNode> root;
		std::vector<ktypes::kval_t> args;
		ktypes::ktype_t returns;
		FnNode(std::string rout, std::vector<ktypes::kval_t> args, ktypes::ktype_t returns, std::shared_ptr<RootNode> rt, int line, int pos_start, int pos_end)
			: name(rout), root(rt), args(args), returns(returns) {
			type = FN;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "fn " << name << std::endl;
			root->print(indent + 1);
		}
	};
	class ReturnNode : public Node {
	public:
		std::shared_ptr<Node> value;
		ReturnNode(std::shared_ptr<Node> value, int line, int pos_start, int pos_end)
			: value(value) {
			type = RETURN;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "return" << std::endl;
			value->print(indent + 1);
		}
	};
	class BreakNode : public Node {
	public:
		BreakNode(int line, int pos_start, int pos_end) {
			type = BREAK;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "break" << std::endl;
		}
	};
	class ContinueNode : public Node {
	public:
		ContinueNode(int line, int pos_start, int pos_end) {
			type = CONTINUE;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "continue" << std::endl;
		}
	};
	class LetNode : public Node {
	public:
		std::string name;
		std::shared_ptr<Node> root;
		bool isAlloc = false;
		int allocVal = -1;
		ktypes::ktype_t varType;
		LetNode(std::string rout, ktypes::ktype_t varType, std::shared_ptr<Node> rt, int line, int pos_start, int pos_end)
			: name(rout), root(rt), varType(varType) {
			type = LET;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		LetNode(std::string rout, ktypes::ktype_t varType, int allocVal, int line, int pos_start, int pos_end)
			: name(rout), allocVal(allocVal), isAlloc(true), varType(varType) {
			type = LET;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "let " << name << std::endl;
			root->print(indent + 1);
		}
	};
	class IndexNode : public Node {
	public:
		std::string name;
		std::shared_ptr<Node> index;
		IndexNode(std::string rout, std::shared_ptr<Node> idx, int line, int pos_start, int pos_end)
			: name(rout), index(idx) {
			type = IDX;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << name << std::endl;
			index->print(indent + 1);
		}
	};
	class VarNode : public Node {
	public:
		std::string name;
		VarNode(std::string rout, int line, int pos_start, int pos_end)
			: name(rout) {
			type = VAR;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << name << std::endl;
		}
	};
	class AddrOfNode : public Node {
	public:
		std::string name;
		AddrOfNode(std::string rout, int line, int pos_start, int pos_end)
			: name(rout) {
			type = ADDROF;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "&" << name << std::endl;
		}
	};
	class DerefNode : public Node {
	public:
		std::string name;
		DerefNode(std::string rout, int line, int pos_start, int pos_end)
			: name(rout) {
			type = DEREF;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "*" << name << std::endl;
		}
	};
	class CompDirectNode : public Node {
	public:
		std::string name;
		int val;
		CompDirectNode(std::string rout, int val, int line, int pos_start, int pos_end)
			: name(rout), val(val) {
			type = CDIRECT;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << name << std::endl;
		}
	};
	class IfNode : public Node {
	public:
		std::shared_ptr<Node> condition, block, else_block;
		bool has_else_block;
		IfNode(std::shared_ptr<Node> condition, std::shared_ptr<Node> block, int line, int pos_start, int pos_end)
			: condition(condition), block(block) {
			type = IF;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "if" << std::endl;
			condition->print(indent + 1);
			block->print(indent + 1);
		}
	};
	class CmpNode : public Node {
	public:
		std::shared_ptr<Node> val1, val2;
		std::map<std::string, std::shared_ptr<RootNode>> comparisons;
		CmpNode(std::shared_ptr<Node> val1, std::shared_ptr<Node> val2, std::map<std::string, std::shared_ptr<RootNode>> comparisons, int line, int pos_start, int pos_end)
			: val1(val1), val2(val2), comparisons(comparisons) {
			type = CMP;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "cmp" << std::endl;
			val1->print(indent + 1);
			val2->print(indent + 1);
			for (std::map<std::string, std::shared_ptr<RootNode>>::const_iterator iter = comparisons.begin(); iter != comparisons.end(); ++iter) {
				std::string k = iter->first;
				std::shared_ptr<RootNode> v = iter->second;
				for (int i = 0; i < indent + 2; i++) std::cout << "--"; std::cout << ' ' << k << std::endl;
				v->print(indent + 3);
			}
		}
	};
	class AsmNode : public Node {
	public:
		std::string content;
		AsmNode(std::string content, int line, int pos_start, int pos_end) : content(content) {
			type = ASM;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "asm <" << content << ">" << std::endl;
		}
	};
	class ForNode : public Node {
	public:
		std::string itername;
		std::shared_ptr<Node> root, initVal, targetVal, stepVal;
		ForNode(std::string itername, std::shared_ptr<Node> root, std::shared_ptr<Node> initVal, std::shared_ptr<Node> targetVal, std::shared_ptr<Node> stepVal, int line, int pos_start, int pos_end)
			: itername(itername), root(root), initVal(initVal), targetVal(targetVal), stepVal(stepVal) {
			type = FOR;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "for " << itername << std::endl;
			initVal->print(indent + 1);
			targetVal->print(indent + 1);
			stepVal->print(indent + 1);
			root->print(indent + 1);
		}
	};
	class LoopNode : public Node {
	public:
		std::shared_ptr<Node> root;
		LoopNode(std::shared_ptr<Node> root, int line, int pos_start, int pos_end)
			: root(root) {
			type = LOOP;
			this->line = line;
			this->pos_start = pos_start;
			this->pos_end = pos_end;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "loop" << std::endl;
			root->print(indent + 1);
		}
	};
}