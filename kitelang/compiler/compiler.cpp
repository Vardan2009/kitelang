#include "compiler.h"

void compiler::Compiler::codegen() {
	for (std::shared_ptr<parser::Node> n : root->statements) {
		if (n->type == parser::FN) {
			std::shared_ptr<parser::FnNode> node = std::static_pointer_cast<parser::FnNode>(n);
			std::vector<ktypes::ktype_t> types;
			for (int i = 0; i < node->args.size(); i++)
				types.push_back(node->args[i].type);
			fns[node->name] = ktypes::kfndec_t{ node->name, types, node->returns, node->is_variadic };
		}
	}
	visit_root(root);
}

std::string compiler::Compiler::txbreg(std::string reg, ktypes::ktype_t type) {
	switch(ktypes::size(type)) {
	case 1:
		return b8r[reg];
	case 2:
		return b16r[reg];
	case 4:
		return b32r[reg];
	case 8:
		return reg;
	}
}

std::string compiler::Compiler::t64breg(std::string reg, ktypes::ktype_t type) {
	return std::string();
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
	case parser::IDX: return visit_idx(std::static_pointer_cast<parser::IndexNode>(node), reg);
	case parser::BINOP: return visit_binop(std::static_pointer_cast<parser::BinOpNode>(node), reg);
	case parser::LET: return visit_let(std::static_pointer_cast<parser::LetNode>(node));
	case parser::ROOT: return visit_root_with_scope(std::static_pointer_cast<parser::RootNode>(node));
	case parser::CMP: return visit_cmp(std::static_pointer_cast<parser::CmpNode>(node));
	case parser::IF: return visit_if(std::static_pointer_cast<parser::IfNode>(node));
	case parser::ASM: return visit_asm(std::static_pointer_cast<parser::AsmNode>(node));
	case parser::FOR: return visit_for(std::static_pointer_cast<parser::ForNode>(node));
	case parser::LOOP: return visit_loop(std::static_pointer_cast<parser::LoopNode>(node));
	case parser::CDIRECT: return visit_cdirect(std::static_pointer_cast<parser::CompDirectNode>(node));
	case parser::ADDROF: return visit_addrof(std::static_pointer_cast<parser::AddrOfNode>(node), reg);
	case parser::DEREF: return visit_deref(std::static_pointer_cast<parser::DerefNode>(node), reg);
	default: throw errors::kiterr("unsupported keyword " + std::to_string(node->type), node->line, node->pos_start, node->pos_end);
	}
}

void compiler::Compiler::visit_root(std::shared_ptr<parser::RootNode> node) {
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
}

void compiler::Compiler::visit_root_with_scope(std::shared_ptr<parser::RootNode> node) {
	std::map<std::string, int> oldvars(varlocs);
	int oldStackSize = stacksize;
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
	textSection.push_back("add rsp, " + std::to_string(stacksize - oldStackSize));
	stacksize = oldStackSize;
	varlocs = oldvars;
}

int compiler::Compiler::visit_root_with_scope_return_amt(std::shared_ptr<parser::RootNode> node) {
	std::map<std::string, int> oldvars(varlocs);
	int oldStackSize = stacksize;
	for (std::shared_ptr<parser::Node> n : node->statements) {
		visit_node(n);
	}
	varlocs = oldvars;
	return (stacksize - oldStackSize);
}

void compiler::Compiler::visit_int_lit(std::shared_ptr<parser::IntLitNode> node, std::string reg) {
	textSection.push_back("mov " + reg + ", " + std::to_string(node->value));
}

void compiler::Compiler::visit_char_lit(std::shared_ptr<parser::CharLitNode> node, std::string reg) {
	textSection.push_back("mov " + reg + ", " + std::to_string(node->value));
}

void compiler::Compiler::visit_reg(std::shared_ptr<parser::RegNode> node, std::string reg) {
	if (node->value == reg) return;
	textSection.push_back("mov " + reg + ", " + node->value);
}

void compiler::Compiler::visit_addrof(std::shared_ptr<parser::AddrOfNode> node, std::string reg) {
	if (varlocs.find(node->name) == varlocs.end())
		throw errors::kiterr("variable " + node->name + " is not present in this context", node->line, node->pos_start, node->pos_end);
	textSection.push_back("lea " + reg + ", [" + "rsp + " + std::to_string(get_variable_offset(node->name)) + "]");
}

void compiler::Compiler::visit_deref(std::shared_ptr<parser::DerefNode> node, std::string reg) {
	if (varlocs.find(node->name) == varlocs.end())
		throw errors::kiterr("variable " + node->name + " is not present in this context", node->line, node->pos_start, node->pos_end);
	textSection.push_back("mov " + reg + ", [" + "rsp + " + std::to_string(get_variable_offset(node->name)) + "]");
	textSection.push_back("mov " + b64r[reg] + ", [" + b64r[reg] + "]");
}

void compiler::Compiler::visit_var(std::shared_ptr<parser::VarNode> node, std::string reg) {
	if (varlocs.find(node->name) == varlocs.end())
		throw errors::kiterr("variable " + node->name + " is not present in this context", node->line, node->pos_start, node->pos_end);
	textSection.push_back("mov " + reg + ", [" + "rsp + " + std::to_string(get_variable_offset(node->name)) + "]");
}

void compiler::Compiler::visit_idx(std::shared_ptr<parser::IndexNode> node, std::string reg) {
	if (varlocs.find(node->name) == varlocs.end())
		throw errors::kiterr("variable " + node->name + " is not present in this context", node->line, node->pos_start, node->pos_end);
	visit_node(node->index, "rbx");
	push("rbx", ktypes::INT64);
	textSection.push_back("mov " + b64r[reg] + ", [" + "rsp + " + std::to_string(get_variable_offset(node->name)) + "]");
	pop("rbx");
	ktypes::ktype_t type = vartypes[node->name];
	if (
		type == ktypes::PTR8 ||
		type == ktypes::PTR16 ||
		type == ktypes::PTR32 ||
		type == ktypes::PTR64
		) {
		int size = 0;
		switch (type) {
		case ktypes::PTR8:
			size = 1;
			break;
		case ktypes::PTR16:
			size = 2;
			break;
		case ktypes::PTR32:
			size = 4;
			break;
		case ktypes::PTR64:
			size = 8;
			break;
		}
		textSection.push_back("imul rbx, rbx, " + std::to_string(size));
	}
	else
		textSection.push_back("imul rbx, rbx, " + std::to_string(ktypes::size(type)));
	textSection.push_back("add " + b64r[reg] + ", rbx");
	textSection.push_back("mov " + b64r[reg] + ", [" + b64r[reg] + "]");
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

	if(fns[node->routine].is_variadic)
		if (fns[node->routine].argtps.size() > node->args.size())
			throw errors::kiterr("wrong amount of arguments given to function " + node->routine + ". expected at least " + std::to_string(fns[node->routine].argtps.size()) + ", got " + std::to_string(node->args.size()), node->line, node->pos_start, node->pos_end);
	
	if (!fns[node->routine].is_variadic)
		if (fns[node->routine].argtps.size() != node->args.size())
			throw errors::kiterr("wrong amount of arguments given to function " + node->routine + ". expected " + std::to_string(fns[node->routine].argtps.size()) + ", got " + std::to_string(node->args.size()), node->line, node->pos_start, node->pos_end);

	for (int i = 0; i < fns[node->routine].argtps.size(); i++) {
		ktypes::ktype_t resultReturn = (fns[node->routine].is_variadic && i >= node->args.size()) ? ktypes::ANY : semantics::would_return(node->args[i], vartypes, fns);

		if (!semantics::compatible(fns[node->routine].argtps[i], resultReturn))
			throw errors::kiterr("function " + node->routine + ", argument " + std::to_string(i + 1) + ": incompatible types " + ktypes::ktype_tn[fns[node->routine].argtps[i]] + " and " + ktypes::ktype_tn[resultReturn], node->args[i]->line, node->args[i]->pos_start, node->args[i]->pos_end);

		visit_node(node->args[i], txbreg("rax", fns[node->routine].argtps[i]));
		push("rax", fns[node->routine].argtps[i]);
	}

	for (int i = node->args.size() - 1; i >= 0; i--)
		pop(argregs[i]);

	textSection.push_back("call " + node->routine);

	if (b64r[reg] != "rax" && reg != "") textSection.push_back("mov " + b64r[reg] + ", rax");
}

void compiler::Compiler::visit_extern(std::shared_ptr<parser::ExternNode> node) {
	for (ktypes::kfndec_t symbol : node->symbols) {
		textSection.push_back("extern " + symbol.name);
		fns[symbol.name] = symbol;
	}
}

void compiler::Compiler::visit_global(std::shared_ptr<parser::GlobalNode> node) {
	for (std::string symbol : node->symbols)
		textSection.push_back("global " + symbol);
}

void compiler::Compiler::visit_return(std::shared_ptr<parser::ReturnNode> node) {
	if(fns[curFn].returns != ktypes::VOID)
		visit_node(node->value, txbreg("rax", fns[curFn].returns));
	textSection.push_back("jmp " + curFn + "_end");
}

void compiler::Compiler::visit_break() {
	textSection.push_back("jmp .loop_end_" + std::to_string(curLoopId));
}

void compiler::Compiler::visit_continue() {
	if (std::shared_ptr<parser::ForNode> forNode = std::dynamic_pointer_cast<parser::ForNode>(curLoop)) {
		visit_node(forNode->stepVal, "rax");
		textSection.push_back("add [rsp + " + std::to_string(get_variable_offset(forNode->itername)) + "], rax");
		textSection.push_back("jmp .loop_" + std::to_string(curLoopId));
	}
	else if (std::shared_ptr<parser::LoopNode> loopNode = std::dynamic_pointer_cast<parser::LoopNode>(curLoop)) {
		textSection.push_back("jmp .loop_" + std::to_string(curLoopId));
	}
}

void compiler::Compiler::visit_fn(std::shared_ptr<parser::FnNode> node) {
	curFn = node->name;
	textSection.push_back(node->name + ":");
	// prepare argument count in rdi and first argument pointer in rsi
	if (node->name == "_start") {
		textSection.push_back("mov rdi, [rsp]");
		textSection.push_back("lea rsi, [rsp + 8]");
	}
	std::map<std::string, int> oldvars(varlocs);
	for (int i = 0; i < node->args.size(); i++) {
		varlocs[node->args[i].name] = stacksize;
		vartypes[node->args[i].name] = node->args[i].type;
		push(argregs[i], node->args[i].type);
	}
	int amtToClear = visit_root_with_scope_return_amt(node->root);
	textSection.push_back(node->name + "_end:");
	textSection.push_back("add rsp, " + std::to_string(amtToClear));
	stacksize -= amtToClear;
	for (int i = 0; i < node->args.size(); i++)
		pop();
	curFn = "";
	varlocs = oldvars;
	// _start is the entry point
	if (node->name == "_start") {
		// for exiting with the return value of _start
		textSection.push_back("mov rdi, rax");
		textSection.push_back("mov rax, 60");
		textSection.push_back("syscall");
	}
	textSection.push_back("ret");
}

void compiler::Compiler::visit_if(std::shared_ptr<parser::IfNode> node) {
	int id = cmpLabelCount++;

	visit_node(node->condition, "rax");
	textSection.push_back("cmp rax, 0");

	textSection.push_back("jne .if_true_" + std::to_string(id));

	if (node->has_else_block)
		textSection.push_back("jmp .if_else_" + std::to_string(id)); 
	else
		textSection.push_back("jmp .if_end_" + std::to_string(id));

	textSection.push_back(".if_true_" + std::to_string(id) + ":");
	visit_node(node->block);

	if (node->has_else_block) {
		textSection.push_back("jmp .if_end_" + std::to_string(id));
		textSection.push_back(".if_else_" + std::to_string(id) + ":");
		visit_node(node->else_block);
	}

	textSection.push_back(".if_end_" + std::to_string(id) + ":");

}

void compiler::Compiler::visit_cmp(std::shared_ptr<parser::CmpNode> node) {
	visit_node(node->val1, "rax");
	push("rax", ktypes::INT64);
	visit_node(node->val2, "rax");
	push("rax", ktypes::INT64);
	pop("rbx");
	pop("rax");
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
	textSection.push_back(".loop_" + std::to_string(id) + ":");
	visit_node(node->root);
	textSection.push_back("jmp .loop_" + std::to_string(id));
	textSection.push_back(".loop_end_" + std::to_string(id) + ":");
}

void compiler::Compiler::visit_for(std::shared_ptr<parser::ForNode> node) {
	int id = cmpLabelCount++;
	curLoop = node;
	curLoopId = id;
	std::map<std::string, int> oldvars(varlocs);

	visit_node(node->initVal, "rax");
	int oldStackSize = stacksize;
	varlocs[node->itername] = stacksize;
	push("rax", ktypes::INT64);

	textSection.push_back(".loop_" + std::to_string(id) + ":");
	if (node->type == parser::ROOT) visit_root(std::static_pointer_cast<parser::RootNode>(node->root));
	else visit_node(node->root);

	visit_node(node->stepVal, "rax");
	textSection.push_back("add [rsp + " + std::to_string(get_variable_offset(node->itername)) + "], rax");
	visit_node(node->targetVal, "rax");
	textSection.push_back("cmp [rsp + " + std::to_string(get_variable_offset(node->itername)) + "], rax");
	textSection.push_back("jg .loop_end_" + std::to_string(id));
	textSection.push_back("jmp .loop_" + std::to_string(id));
	textSection.push_back(".loop_end_" + std::to_string(id) + ": ");
	textSection.push_back("add rsp, " + std::to_string(stacksize - oldStackSize));
	stacksize = oldStackSize;
	varlocs = oldvars;
}

void compiler::Compiler::visit_let(std::shared_ptr<parser::LetNode> node) {
	if (node->isAlloc) {
		int allocationSize = node->allocVal * ktypes::size(node->varType);

		if (allocationSize > 0) {
			int totalAllocation = (allocationSize + 15) & ~15; // Align to 16 bytes
			textSection.push_back("sub rsp, " + std::to_string(totalAllocation));
			stacksize += totalAllocation;
		}
		varlocs[node->name] = stacksize;
		switch (ktypes::size(node->varType)) {
		case 0:
			throw errors::kiterr("cannot create array with void type", node->line, node->pos_start, node->pos_end);
		case 1:
			vartypes[node->name] = ktypes::PTR8;
			break;
		case 2:
			vartypes[node->name] = ktypes::PTR16;
			break;
		case 4:
			vartypes[node->name] = ktypes::PTR32;
			break;
		case 8:
			vartypes[node->name] = ktypes::PTR64;
			break;
		}
		push("rsp", ktypes::INT64);
	}
	else {
		ktypes::ktype_t resultReturn = semantics::would_return(node->root, vartypes, fns);
		if(!semantics::compatible(node->varType, resultReturn))
			throw errors::kiterr("incompatible types " + ktypes::ktype_tn[node->varType] + " and " + ktypes::ktype_tn[resultReturn], node->line, node->pos_start, node->pos_end);

		visit_node(node->root, txbreg("rax", node->varType));
		vartypes[node->name] = node->varType;
		varlocs[node->name] = stacksize;
		push("rax", node->varType);
	}
}

void compiler::Compiler::visit_cdirect(std::shared_ptr<parser::CompDirectNode> node) {
	if (node->name == "stackszinc")
		stacksize += node->val;
	else if (node->name == "stackszdec")
		stacksize -= node->val;
	else
		throw errors::kiterr("Invalid compiler directive " + node->name, node->line, node->pos_start, node->pos_end);
}

// this part is VERY complicated
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
				push("rax", ktypes::INT64);
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
	else if (node->operation == lexer::MUL || node->operation == lexer::DIV || node->operation == lexer::MOD) {
		// Multiplication or division always has precedence
		visit_node(node->left, "rax");
		push("rax", ktypes::INT64);
		visit_node(node->right, "rbx");
		pop("rax");

		if (node->operation == lexer::MUL) {
			textSection.push_back("imul rax, rbx");
		}
		else if (node->operation == lexer::DIV) {
			textSection.push_back("xor rdx, rdx");  // Clear rdx for division
			textSection.push_back("idiv rbx");
		}
		else if (node->operation == lexer::MOD) {
			textSection.push_back("xor rdx, rdx");
			textSection.push_back("idiv rbx");     
			textSection.push_back("mov rax, rdx");
		}
	}
	// Handle comparison operators
	else if (node->operation == lexer::EQEQ || node->operation == lexer::NEQEQ ||
		node->operation == lexer::GT || node->operation == lexer::LT ||
		node->operation == lexer::GTE || node->operation == lexer::LTE) {

		visit_node(node->left, "rax");
		push("rax", ktypes::INT64);
		visit_node(node->right, "rbx");
		pop("rax");

		int id = cmpLabelCount++;
		std::string label_true = ".boolop_true_" + std::to_string(id);
		std::string label_end = ".boolop_end_" + std::to_string(id);

		// Perform comparison
		if (node->operation == lexer::EQEQ) {
			textSection.push_back("cmp rax, rbx");
			textSection.push_back("je " + label_true);
		}
		else if (node->operation == lexer::NEQEQ) {
			textSection.push_back("cmp rax, rbx");
			textSection.push_back("jne " + label_true);
		}
		else if (node->operation == lexer::GT) {
			textSection.push_back("cmp rax, rbx");
			textSection.push_back("jg " + label_true);
		}
		else if (node->operation == lexer::LT) {
			textSection.push_back("cmp rax, rbx");
			textSection.push_back("jl " + label_true);
		}
		else if (node->operation == lexer::GTE) {
			textSection.push_back("cmp rax, rbx");
			textSection.push_back("jge " + label_true);
		}
		else if (node->operation == lexer::LTE) {
			textSection.push_back("cmp rax, rbx");
			textSection.push_back("jle " + label_true);
		}

		// If the condition is false, jump to the end
		textSection.push_back("mov rax, 0");
		textSection.push_back("jmp " + label_end);
		textSection.push_back(label_true + ":");
		textSection.push_back("mov rax, 1"); // True condition
		textSection.push_back(label_end + ":");
	}
	else if (node->operation == lexer::EQ) { // Assignment
		visit_node(node->right, "rax"); // store the new value in rax
		if (node->left->type == parser::VAR) // regular variable (x)
			textSection.push_back("mov [rsp + " + std::to_string(get_variable_offset(std::static_pointer_cast<parser::VarNode>(node->left)->name)) + "], " + txbreg("rax", vartypes[std::static_pointer_cast<parser::VarNode>(node->left)->name])); // move the result from rax to the stack
		else if (node->left->type == parser::DEREF) { // variable dereference pointer (*x)
			ktypes::ktype_t type = vartypes[std::static_pointer_cast<parser::DerefNode>(node->left)->name];
			if (
				type != ktypes::PTR8  &&
				type != ktypes::PTR16 &&
				type != ktypes::PTR32 &&
				type != ktypes::PTR64
				)
				throw errors::kiterr("cannot dereference a non-pointer", node->left->line, node->left->pos_start, node->left->pos_end);
			textSection.push_back("mov rbx, [rsp + " + std::to_string(get_variable_offset(std::static_pointer_cast<parser::DerefNode>(node->left)->name)) +"]");
			textSection.push_back("mov [rbx], rax");
		}
		else if (node->left->type == parser::IDX) {  // index access pointer (x[i])
			std::shared_ptr<parser::IndexNode> n = std::static_pointer_cast<parser::IndexNode>(node->left);
			push("rax", ktypes::INT64);
			visit_node(n->index, "rcx");
			textSection.push_back("mov rbx, [rsp + " + std::to_string(get_variable_offset(n->name)) + "]");
			ktypes::ktype_t type = vartypes[std::static_pointer_cast<parser::IndexNode>(node->left)->name];
			if (
				type == ktypes::PTR8  ||
				type == ktypes::PTR16 ||
				type == ktypes::PTR32 ||
				type == ktypes::PTR64
				) {
				int size = 0;
				switch (type) {
				case ktypes::PTR8:
					size = 1;
					break;
				case ktypes::PTR16:
					size = 2;
					break;
				case ktypes::PTR32:
					size = 4;
					break;
				case ktypes::PTR64:
					size = 8;
					break;
				}
				textSection.push_back("imul rcx, rcx, " + std::to_string(size));
			}
			else
				textSection.push_back("imul rcx, rcx, " + std::to_string(ktypes::size(type)));
			textSection.push_back("add rbx, rcx");
			pop("rax");
			textSection.push_back("mov [rbx], rax");
		}
		else throw errors::kiterr("invalid lhs of assignment", node->left->line, node->left->pos_start, node->left->pos_end);
	}

	// Store the result in the appropriate register
	if (node->operation != lexer::EQ)
		textSection.push_back("mov " + b64r[reg] + ", rax");
}


int compiler::Compiler::get_variable_offset(std::string varname) {
	return (stacksize - 8 - varlocs[varname]);
}

void compiler::Compiler::push(std::string reg, ktypes::ktype_t type) {
	// apparently using movzx for moving from 32 bit register to 64 bit
	// will result in an error
	// In 64-bit code, when the destination operand is a 32-bit register
	// the CPU automatically zero extends the result through the upper 32-bits of the 64-bit register.
	// That's why the [ktypes::size(type) != 4] condition is here
	if (reg != txbreg(reg, type) && ktypes::size(type) != 4)
		textSection.push_back("movzx " + reg + ", " + txbreg(reg, type));
	textSection.push_back("push " + reg);
	stacksize += 8;
}

void compiler::Compiler::pop(std::string reg) {
	textSection.push_back("pop " + reg);
	stacksize -= 8;
}

void compiler::Compiler::pop() {
	textSection.push_back("add rsp, 8");
	stacksize -= 8;
}