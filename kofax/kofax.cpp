#include "node.h"

#include <iostream>
#include <fstream>
#include <string>

int main() {
	Context context;
	std::string input;
	AST ast;
	while (!std::cin.eof()) {
		std::getline(std::cin, input);
		if (input == "calculate") {
			std::cout << ast.calculate(context) << std::endl;
		}
		else {
			size_t asg = input.find('=');
			if (asg == std::string::npos) {
				AST new_ast;
				new_ast.parse(input);
				if (new_ast.error()) {
					std::cout << "Expression has not been set. " << new_ast.calculate(context) << std::endl;
				}
				else {
					std::cout << "Expression has been set" << std::endl;
					ast.swap(new_ast);
				}
			}
			else {
				if (asg == 0) {
					std::cout << "Empty variable" << std::endl;
					continue;

				}
				std::string expr = input.substr(asg + 1);
				asg--;
				while (asg >= 0 && input[asg] == ' ') {
					asg--;
				}
				bool good = asg >= 0 && isalpha(input[0]);
				for (size_t i = 1; i <= asg; i++) {
					good = good && isalnum(input[i]) > 0;
				}
				if (!good) {
					std::cout << "Invalid variable" << std::endl;
					continue;
				}
				std::string var = input.substr(0, asg + 1);
				AST value;
				value.parse(expr);
				if (value.error()) {
					std::cout << "Invalid value. " << value.errorMessage() << std::endl;
					continue;
				}
				Result res = value.calculate(context);
				if (res.error()) {
					std::cout << "Invalid expression. " << res << std::endl;
					continue;
				}
				if (context.in(var)) {
					std::cout << "Variable " + var + " has been changed" << std::endl;
				}
				else {
					std::cout << "Variable " + var + " has been added" << std::endl;
				}
				context.set(var, res.number);
			}
		}
	}
	
    return 0;
}

