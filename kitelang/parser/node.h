#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "../lexer/token.h"

namespace parser {
	typedef enum {
		ROOT,
		BINOP,
		STRING_LIT,
		INT_LIT,
		EXTERN,
		GLOBAL,
		ROUTINE,
		CALL,
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
	class RoutineNode : public Node {
	public:
		std::string name;
		std::shared_ptr<RootNode> root;
		RoutineNode(std::string rout, std::shared_ptr<RootNode> rt)
			: name(rout), root(rt) {
			type = ROUTINE;
		}
		void print(int indent = 0) const {
			for (int i = 0; i < indent; i++) std::cout << "--"; std::cout << ' ';
			std::cout << "routine " << name << std::endl;
			root->print(indent + 1);
		}
	};
}