#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <map>

#include "../lexer/token.h"

namespace parser {
	typedef enum {
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
		VAR,
		CMP,
		ASM,
		FOR,
		LOOP,
		CDIRECT
	} node_t;
	class Node {
	public:
		node_t type;
		virtual void print(int) const = 0;
	};
	class RootNode : public Node {
	public:
		std::vector<std::shared_ptr<Node>> statements;
		RootNode(std::vector< std::shared_ptr<Node>> stmts)
			: statements(stmts) {
			type = ROOT;
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
		BinOpNode(std::shared_ptr<Node> l, lexer::token_t op, std::shared_ptr<Node> r)
			: left(l), right(r), operation(op) {
			type = BINOP;
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
		CharLitNode(char val)
			: value(val) {
			type = CHAR_LIT;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << value << std::endl;
		}
	};
	class RegNode : public Node {
	public:
		std::string value;
		RegNode(std::string val)
			: value(val) {
			type = REG;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "%" << value << std::endl;
		}
	};
	class StringLitNode : public Node {
	public:
		std::string value;
		StringLitNode(std::string val)
			: value(val) {
			type = STRING_LIT;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << value << std::endl;
		}
	};
	class IntLitNode : public Node {
	public:
		int value;
		IntLitNode(int val)
			: value(val) {
			type = INT_LIT;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << value << std::endl;
		}
	};
	class ExternNode : public Node {
	public:
		std::string routine;
		ExternNode(std::string rout)
			: routine(rout) {
			type = EXTERN;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "extern " << routine << std::endl;
		}
	};
	class GlobalNode : public Node {
	public:
		std::string routine;
		GlobalNode(std::string rout)
			: routine(rout) {
			type = GLOBAL;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "global " << routine << std::endl;
		}
	};
	class CallNode : public Node {
	public:
		std::string routine;
		std::vector<std::shared_ptr<Node>> args;
		CallNode(std::string rout, std::vector<std::shared_ptr<Node>> a)
			: routine(rout), args(a) {
			type = CALL;
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
		std::vector<std::string> argnames;
		FnNode(std::string rout, std::vector<std::string> argnms, std::shared_ptr<RootNode> rt)
			: name(rout), root(rt), argnames(argnms) {
			type = FN;
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
		ReturnNode(std::shared_ptr<Node> value)
			: value(value) {
			type = RETURN;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "return" << std::endl;
			value->print(indent + 1);
		}
	};
	class BreakNode : public Node {
	public:
		BreakNode() {
			type = BREAK;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "break" << std::endl;
		}
	};
	class ContinueNode : public Node {
	public:
		ContinueNode() {
			type = CONTINUE;
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
		LetNode(std::string rout, std::shared_ptr<Node> rt)
			: name(rout), root(rt) {
			type = LET;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "let " << name << std::endl;
			root->print(indent + 1);
		}
	};
	class VarNode : public Node {
	public:
		std::string name;
		VarNode(std::string rout)
			: name(rout) {
			type = VAR;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << name << std::endl;
		}
	};
	class AddrOfNode : public Node {
	public:
		std::string name;
		AddrOfNode(std::string rout)
			: name(rout) {
			type = ADDROF;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "&" << name << std::endl;
		}
	};
	class DerefNode : public Node {
	public:
		std::string name;
		DerefNode(std::string rout)
			: name(rout) {
			type = DEREF;
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
		CompDirectNode(std::string rout, int val)
			: name(rout), val(val) {
			type = CDIRECT;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << name << std::endl;
		}
	};
	class CmpNode : public Node {
	public:
		std::shared_ptr<Node> val1, val2;
		std::map<std::string, std::shared_ptr<RootNode>> comparisons;
		CmpNode(std::shared_ptr<Node> val1, std::shared_ptr<Node> val2, std::map<std::string, std::shared_ptr<RootNode>> comparisons)
			: val1(val1), val2(val2), comparisons(comparisons) {
			type = CMP;
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
		AsmNode(std::string content) : content(content) {
			type = ASM;
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
		ForNode(std::string itername, std::shared_ptr<Node> root, std::shared_ptr<Node> initVal, std::shared_ptr<Node> targetVal, std::shared_ptr<Node> stepVal)
			: itername(itername), root(root), initVal(initVal), targetVal(targetVal), stepVal(stepVal) {
			type = FOR;
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
		LoopNode(std::shared_ptr<Node> root)
			: root(root) {
			type = LOOP;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "loop" << std::endl;
			root->print(indent + 1);
		}
	};
}