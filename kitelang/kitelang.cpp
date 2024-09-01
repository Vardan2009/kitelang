#include "kitelang.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "kite: usage: kite (path/to/file.kite)" << std::endl;
		return 1;
	}

	std::ifstream file(argv[1]);
	std::string src;

	if (!file.is_open() || !file) {
		std::cerr << "kite: failed to open file" << std::endl;
		return 1;
	}

	std::ostringstream ss;
	ss << file.rdbuf();
	src = ss.str();

	std::vector<token_ptr> tokens;

	try {
		lexer::Lexer lex(src);
		tokens = lex.tokenize();
	}
	catch (std::runtime_error e) {
		std::cerr << "kite: lexer: " << e.what() << std::endl;
		exit(1);
	}

	//for (int i = 0; i < tokens.size(); i++) {
	//	std::cout << i << ": TOKEN(" << tokens[i]->type << ", " << tokens[i]->value << ", " << tokens[i]->value_str << ")" << std::endl;
	//}

	std::shared_ptr<parser::RootNode> root;
	parser::Parser parser(tokens);
	try {
		root = parser.parse();
	} catch (std::runtime_error e) {
		std::cerr << "kite: parser: " << e.what() << " at token index " << parser.ptr << std::endl;
		exit(1);
	}

	root->print(0);

	compiler::Compiler compiler(root);
	try {
		compiler.codegen();
	} catch (std::runtime_error e) {
		std::cerr << "kite: compiler: " << e.what() << std::endl;
	}
	compiler.print();

	return 0;
}
