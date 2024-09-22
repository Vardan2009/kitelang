#include "compiler.h"

void compiler::Compiler::codegen() {
	visit_root(root);
}

void compiler::Compiler::visit_node(std::shared_ptr<parser::Node> node, std::string reg) {
	switch (node->type) {
	case parser::EXTERN: return visit_extern(std::static_pointer_cast<parser::ExternNode>(node));
	case parser::GLOBAL: return visit_global(std::static_pointer_cast<parser::GlobalNode>(node));
	case parser::CALL: return visit_call(std::static_pointer_cast<parser::CallNode>(node), reg);
	case parser::FN: return visit_fn(std::static_pointer_cast<parser::FnNode>(node));
	case parser::RETURN: return visit_return(std::static_pointer_cast<parser::ReturnNode>(node));
	case parser::BREAK: return visit_break();
	case parser::CONTINUE: return visit_continue();
	case parser::INT_LIT: return visit_int_lit(std::static_pointer_cast<parser::IntLitNode>(node), reg);
	case parser::CHAR_LIT: return visit_char_lit(std::static_pointer_cast<parser::CharLitNode>(node), reg);
	case parser::REG: return visit_reg(std::static_pointer_cast<parser::RegNode>(node), reg);
	case parser::STRING_LIT: return visit_string_lit(std::static_pointer_cast<parser::StringLitNode>(node), reg);
	case parser::VAR: return visit_var(std::static_pointer_cast<parser::VarNode>(node), reg);
	case parser::BINOP: return visit_binop(std::static_pointer_cast<parser::BinOpNode>(node), reg);
	case parser::LET: return visit_let(std::static_pointer_cast<parser::LetNode>(node));
	case parser::ROOT: return visit_root_with_scope(std::static_pointer_cast<parser::RootNode>(node));
	case parser::CMP: return visit_cmp(std::static_pointer_cast<parser::CmpNode>(node));
	case parser::ASM: return visit_asm(std::static_pointer_cast<parser::AsmNode>(node));
	case parser::FOR: return visit_for(std::static_pointer_cast<parser::ForNode>(node));
	case parser::LOOP: return visit_loop(std::static_pointer_cast<parser::LoopNode>(node));
	default: throw std::runtime_error("yet unsupported keyword " + std::to_string(node->type));
	}
}

void compiler::Compiler::visit_root(std::shared_ptr<parser::RootNode> node) {
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
}

void compiler::Compiler::visit_root_with_scope(std::shared_ptr<parser::RootNode> node) {
	std::map<std::string, int> oldvars(vars);
	int oldStackSize = stacksize;
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
	for (int i = stacksize; i > oldStackSize; i--) {
		pop();
	}
	vars = oldvars;
}

void compiler::Compiler::visit_int_lit(std::shared_ptr<parser::IntLitNode> node, std::string reg) {
	textSection.push_back("mov " + reg + ", " + std::to_string(node->value));
}

void compiler::Compiler::visit_char_lit(std::shared_ptr<parser::CharLitNode> node, std::string reg) {
	dataSection.push_back("datasec_" + std::to_string(dataSectionCount) + " db " + std::to_string(node->value));
	textSection.push_back("mov " + reg + ", datasec_" + std::to_string(dataSectionCount));
	++dataSectionCount;
}

void compiler::Compiler::visit_reg(std::shared_ptr<parser::RegNode> node, std::string reg) {
	if (node->value == reg) return;
	textSection.push_back("mov " + reg + ", " + node->value);
}

void compiler::Compiler::visit_var(std::shared_ptr<parser::VarNode> node, std::string reg) {
	if (vars.find(node->name) == vars.end())
		throw std::runtime_error("variable " + node->name + " is not present in this context");
	textSection.push_back("mov " + reg + ", [" + "rsp + " + std::to_string(get_variable_offset(node->name)) + "]");
}

void compiler::Compiler::visit_string_lit(std::shared_ptr<parser::StringLitNode> node, std::string reg) {
	std::string processedLiteral;

	for (size_t i = 0; i < node->value.length(); ++i) {
		if (node->value[i] == '\\' && i + 1 < node->value.length()) {
			switch (node->value[i + 1]) {
			case 'n': processedLiteral += "\", 10, \""; break;  // newline
			case '0': processedLiteral += "\", 0, \""; break;   // null-terminator
			case 't': processedLiteral += "\", 9, \""; break;   // horizontal tab
			case 'r': processedLiteral += "\", 13, \""; break;  // carriage return
			case '\\': processedLiteral += "\\\\"; break;       // backslash
			case '\"': processedLiteral += "\", 34, \""; break; // double quote
			default:
				processedLiteral += node->value[i + 1];
				break;
			}
			++i;
		}
		else {
			processedLiteral += node->value[i];
		}
	}

	dataSection.push_back("datasec_" + std::to_string(dataSectionCount) + " db \"" + processedLiteral + "\", 0");
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

void compiler::Compiler::visit_return(std::shared_ptr<parser::ReturnNode> node) {
	visit_node(node->value, "rax");
	textSection.push_back("jmp " + curFn + "_end");
}

void compiler::Compiler::visit_break() {
	textSection.push_back("jmp loop_end_" + std::to_string(curLoopId));
}

void compiler::Compiler::visit_continue() {
	if (std::shared_ptr<parser::ForNode> forNode = std::dynamic_pointer_cast<parser::ForNode>(curLoop)) {
		visit_node(forNode->stepVal, "rax");
		textSection.push_back("add [rsp + " + std::to_string(get_variable_offset(forNode->itername)) + "], rax");
		textSection.push_back("jmp loop_" + std::to_string(curLoopId));
	}
	else if (std::shared_ptr<parser::LoopNode> loopNode = std::dynamic_pointer_cast<parser::LoopNode>(curLoop)) {
		textSection.push_back("jmp loop_" + std::to_string(curLoopId));
	}
}

void compiler::Compiler::visit_fn(std::shared_ptr<parser::FnNode> node) {
	curFn = node->name;
	textSection.push_back(node->name + ":");
	std::map<std::string, int> oldvars(vars);
	for (int i = 0; i < node->argnames.size(); i++) {
		vars[node->argnames[i]] = stacksize;
		push(argregs[i]);
	}
	visit_root_with_scope(node->root);
	textSection.push_back(node->name + "_end:");
	for (int i = 0; i < node->argnames.size(); i++) {
		pop();
	}
	curFn = "";
	vars = oldvars;
	// _start is the entry point
	if (node->name == "_start") {
		// for exiting with the return value of _start
		textSection.push_back("mov rdi, rax");
		textSection.push_back("mov rax, 60");
		textSection.push_back("syscall");
	}
	textSection.push_back("ret");
}

void compiler::Compiler::visit_cmp(std::shared_ptr<parser::CmpNode> node) {
	visit_node(node->val1, "rax");
	visit_node(node->val2, "rbx");
	textSection.push_back("cmp rax, rbx");
	int id = cmpLabelCount++;
	for (std::map<std::string, std::shared_ptr<parser::RootNode>>::const_iterator iter = node->comparisons.begin(); iter != node->comparisons.end(); ++iter) {
		std::string k = iter->first;
		textSection.push_back(cmpkeywordinstruction[k] + " " + k + "_block_" + std::to_string(id));
	}
	textSection.push_back("jmp end_" + std::to_string(id));
	for (std::map<std::string, std::shared_ptr<parser::RootNode>>::const_iterator iter = node->comparisons.begin(); iter != node->comparisons.end(); ++iter) {
		std::string k = iter->first;
		std::shared_ptr<parser::RootNode> root = iter->second;
		textSection.push_back(k + "_block_" + std::to_string(id) + ":");
		visit_node(root);
		textSection.push_back("jmp end_" + std::to_string(id));
	}
	textSection.push_back("end_" + std::to_string(id) + ": ");

}

void compiler::Compiler::visit_asm(std::shared_ptr<parser::AsmNode> node) {
	textSection.push_back(node->content);
}

void compiler::Compiler::visit_loop(std::shared_ptr<parser::LoopNode> node) {
	int id = cmpLabelCount++;
	curLoop = node;
	curLoopId = id;
	textSection.push_back("loop_" + std::to_string(id) + ":");
	visit_node(node->root);
	textSection.push_back("jmp loop_" + std::to_string(id));
	textSection.push_back("loop_end_" + std::to_string(id) + ":");
}

void compiler::Compiler::visit_for(std::shared_ptr<parser::ForNode> node) {
	int id = cmpLabelCount++;
	curLoop = node;
	curLoopId = id;
	std::map<std::string, int> oldvars(vars);

	visit_node(node->initVal, "rax");
	int oldStackSize = stacksize;
	vars[node->itername] = stacksize;
	push("rax");

	textSection.push_back("loop_" + std::to_string(id) + ":");
	if (node->type == parser::ROOT) visit_root(std::static_pointer_cast<parser::RootNode>(node->root));
	else visit_node(node->root);

	visit_node(node->stepVal, "rax");
	textSection.push_back("add [rsp + " + std::to_string(get_variable_offset(node->itername)) + "], rax");
	visit_node(node->targetVal, "rax");
	textSection.push_back("cmp [rsp + " + std::to_string(get_variable_offset(node->itername)) + "], rax");
	textSection.push_back("jg loop_end_" + std::to_string(id));
	textSection.push_back("jmp loop_" + std::to_string(id));
	textSection.push_back("loop_end_" + std::to_string(id) + ": ");
	for (int i = stacksize; i > oldStackSize; i--) {
		pop();
	}
	vars = oldvars;
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
	if(node->operation != lexer::EQ)
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