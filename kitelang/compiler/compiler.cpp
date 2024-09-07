#include "compiler.h"

void compiler::Compiler::codegen() {
	visit_root(root);
}

void compiler::Compiler::visit_node(std::shared_ptr<parser::Node> node, std::string reg) {
	switch (node->type) {
	case parser::EXTERN: return visit_extern(std::static_pointer_cast<parser::ExternNode>(node));
	case parser::GLOBAL: return visit_global(std::static_pointer_cast<parser::GlobalNode>(node));
	case parser::CALL: return visit_call(std::static_pointer_cast<parser::CallNode>(node), reg);
	case parser::ROUTINE: return visit_routine(std::static_pointer_cast<parser::RoutineNode>(node));
	case parser::INT_LIT: return visit_int_lit(std::static_pointer_cast<parser::IntLitNode>(node), reg);
	case parser::STRING_LIT: return visit_string_lit(std::static_pointer_cast<parser::StringLitNode>(node), reg);
	case parser::VAR: return visit_var(std::static_pointer_cast<parser::VarNode>(node), reg);
	case parser::BINOP: return visit_binop(std::static_pointer_cast<parser::BinOpNode>(node), reg);
	case parser::LET: return visit_let(std::static_pointer_cast<parser::LetNode>(node));
	case parser::ROOT: return visit_root_with_scope(std::static_pointer_cast<parser::RootNode>(node));
	default: throw std::runtime_error("yet unsupported keyword " + std::to_string(node->type));
	}
}

void compiler::Compiler::visit_root(std::shared_ptr<parser::RootNode> node) {
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
}

void compiler::Compiler::visit_root_with_scope(std::shared_ptr<parser::RootNode> node) {
	textSection.push_back("; SCOPE START");
	std::map<std::string, int> oldvars(vars);
	int oldStackSize = stacksize;
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
	textSection.push_back("; SCOPE END");
	for (int i = stacksize; i > oldStackSize; i--) {
		pop();
	}
	vars = oldvars;
	textSection.push_back("; SCOPE VALUES DISCARDED");
}

void compiler::Compiler::visit_int_lit(std::shared_ptr<parser::IntLitNode> node, std::string reg) {
	textSection.push_back("mov " + reg + ", " + std::to_string(node->value));
}

void compiler::Compiler::visit_var(std::shared_ptr<parser::VarNode> node, std::string reg) {
	if (vars.find(node->name) == vars.end())
		throw std::runtime_error("variable " + node->name + " is not present in this context");
	textSection.push_back("mov " + reg + ", [" + "rsp + " + std::to_string(get_variable_offset(node->name)) + "]");
}

void compiler::Compiler::visit_string_lit(std::shared_ptr<parser::StringLitNode> node, std::string reg) {
	dataSection.push_back("datasec_" + std::to_string(dataSectionCount) + " db \"" + node->value + "\", 0");
	textSection.push_back("mov " + reg + ", datasec_" + std::to_string(dataSectionCount));
	++dataSectionCount;
}

void compiler::Compiler::visit_call(std::shared_ptr<parser::CallNode> node, std::string reg) {
	// for (int i = node->args.size(); i < 6; i++) {
	//	 textSection.push_back("xor " + argregs[i] + ", " + argregs[i]);
	// }
	for (int i = 0; i < node->args.size(); i++) {
		visit_node(node->args[i], argregs[i]);
	}
	textSection.push_back("call " + node->routine);

	if (reg != "rax" && reg != "")
		textSection.push_back("mov " + reg + ", rax");
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

void compiler::Compiler::visit_let(std::shared_ptr<parser::LetNode> node) {
	visit_node(node->root, "rax");
	vars[node->name] = stacksize;
	push("rax");
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
				push("rax");
				visit_binop(right_binop, "rax");  // Evaluate the right child expression first
				pop("rbx"); // Restore the left-hand side
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
	else if (node->operation == lexer::EQ) { // if the operation is assignment <varname> = <expr>
		visit_node(node->right, "rax"); // store the new value in rax
		if (node->left->type != parser::VAR)
			throw std::runtime_error("LHS of assignment should be variable");

		textSection.push_back("mov [rsp + " + std::to_string(get_variable_offset(std::static_pointer_cast<parser::VarNode>(node->left)->name)) + "], rax"); // move the result from rax to the stack
	}

	// Store the result in the appropriate register
	textSection.push_back("mov " + reg + ", rax");
}

int compiler::Compiler::get_variable_offset(std::string varname) {
	return (stacksize - 1 - vars[varname]) * 8;
}

void compiler::Compiler::push(std::string reg) {
	textSection.push_back("push " + reg);
	stacksize++;
}

void compiler::Compiler::pop(std::string reg) {
	textSection.push_back("pop " + reg);
	stacksize--;
}

void compiler::Compiler::pop() {
	textSection.push_back("add rsp, 8");
	stacksize--;
}