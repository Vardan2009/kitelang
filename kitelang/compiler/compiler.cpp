#include "compiler.h"

void compiler::Compiler::codegen() {
	visit_root(root);
}

void compiler::Compiler::visit_node(std::shared_ptr<parser::Node> node, std::string reg) {
	switch (node->type) {
	case parser::EXTERN: return visit_extern(std::static_pointer_cast<parser::ExternNode>(node));
	case parser::GLOBAL: return visit_global(std::static_pointer_cast<parser::GlobalNode>(node));
	case parser::CALL: return visit_call(std::static_pointer_cast<parser::CallNode>(node));
	case parser::ROUTINE: return visit_routine(std::static_pointer_cast<parser::RoutineNode>(node));
	case parser::INT_LIT: return visit_int_lit(std::static_pointer_cast<parser::IntLitNode>(node), reg);
	case parser::STRING_LIT: return visit_string_lit(std::static_pointer_cast<parser::StringLitNode>(node), reg);
	case parser::BINOP: return visit_binop(std::static_pointer_cast<parser::BinOpNode>(node), reg);
	default: throw std::runtime_error("yet unsupported keyword");
	}
}

void compiler::Compiler::visit_root(std::shared_ptr<parser::RootNode> node) {
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
}

void compiler::Compiler::visit_int_lit(std::shared_ptr<parser::IntLitNode> node, std::string reg) {
	textSection.push_back("mov " + reg + ", " + std::to_string(node->value));
}

void compiler::Compiler::visit_string_lit(std::shared_ptr<parser::StringLitNode> node, std::string reg) {
	dataSection.push_back("datasec_" + std::to_string(dataSectionCount) + " db \"" + node->value + "\", 0");
	textSection.push_back("mov " + reg + ", datasec_" + std::to_string(dataSectionCount));
	++dataSectionCount;
}

void compiler::Compiler::visit_call(std::shared_ptr<parser::CallNode> node) {
	for (int i = 0; i < node->args.size(); i++) {
		visit_node(node->args[i], argregs[i]);
	}
	textSection.push_back("call " + node->routine);
}

void compiler::Compiler::visit_extern(std::shared_ptr<parser::ExternNode> node) {
	textSection.push_back("extern " + node->routine);
}

void compiler::Compiler::visit_global(std::shared_ptr<parser::GlobalNode> node) {
	textSection.push_back("global " + node->routine);
}

void compiler::Compiler::visit_routine(std::shared_ptr<parser::RoutineNode> node) {
	textSection.push_back(node->name + ":");
	visit_root(node->root);
	textSection.push_back("ret");
}

// this part is kinda complicated
void compiler::Compiler::visit_binop(std::shared_ptr<parser::BinOpNode> node, std::string reg) {
	// Check operator precedence
	if (node->operation == lexer::PLUS || node->operation == lexer::MINUS) {
		// Left child is evaluated first
		visit_node(node->left, "rax");

		// If right child is a multiplication or division, evaluate it first to respect precedence
		if (node->right->type == parser::BINOP) {
			auto right_binop = std::static_pointer_cast<parser::BinOpNode>(node->right);
			if (right_binop->operation == lexer::MUL || right_binop->operation == lexer::DIV) {
				// Temporarily store the result of the left side
				textSection.push_back("push rax");
				visit_binop(right_binop, "rax");  // Evaluate the right child expression first
				textSection.push_back("pop rbx"); // Restore the left-hand side
			}
			else {
				visit_node(node->right, "rbx");  // Direct evaluation for non-precedence cases
			}
		}
		else {
			visit_node(node->right, "rbx");
		}

		// Perform the actual operation
		if (node->operation == lexer::PLUS) {
			textSection.push_back("add rax, rbx");
		}
		else if (node->operation == lexer::MINUS) {
			textSection.push_back("sub rax, rbx");
		}
	}
	else if (node->operation == lexer::MUL || node->operation == lexer::DIV) {
		// Multiplication or division always has precedence
		visit_node(node->left, "rax");
		visit_node(node->right, "rbx");

		if (node->operation == lexer::MUL) {
			textSection.push_back("imul rax, rbx");
		}
		else if (node->operation == lexer::DIV) {
			textSection.push_back("xor rdx, rdx");  // Clear rdx for division
			textSection.push_back("idiv rbx");
		}
	}

	// Store the result in the appropriate register
	textSection.push_back("mov " + reg + ", rax");
}