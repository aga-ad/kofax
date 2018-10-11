#include "node.h"

#include <vector>
#include <iostream>
#include <memory>


Token::Token(const Token& token) {
	type = token.type;
	if (type == Type::Error || type == Type::Var) {
		new (&str) std::string(token.str);
	}
	else if (type == Type::Number) {
		number = token.number;
	}
};

Token::Token(Type type) : type(type) {};

Token::Token(Type type, std::string s): type(type) {
	new (&str) std::string(s);
}

Token::Token(Type type, num num) : type(type) {
	number = num;
}

int Token::priority() const {
	if (type == Type::Var) {
		return 0;
	}
	else if (type == Type::Bracket) {
		return -100;
	}
	else if (type == Type::Add || type == Type::Sub) {
		return 1;
	}
	else if (type == Type::Mul || type == Type::Div) {
		return 2;
	}
	else {
		return -1;
	}
}

bool Token::isOp2() const {
	return type == Type::Add || type == Type::Sub || type == Type::Mul || type == Type::Div;
}

Token::~Token() {
	if (type == Type::Error || type == Type::Var) {
		str.~basic_string<char>();
	}
}


Result::Result(std::string message) : message(message) {};

Result::Result(num number) : number(number) {};

bool Result::error() const {
	return message.length() > 0;
}

std::ostream& operator<<(std::ostream& os, const Result& res) {
	if (res.error()) {
		os << "Error: " + res.message;
	}
	else {
		os << res.number;
	}
	return os;
}


Result Context::get(const std::string var) const {
	if (variables.count(var) > 0) {
		return Result(variables.at(var));
	}
	else {
		return Result("Unknown variable " + var);
	}
}

bool Context::in(const std::string var) const {
	return variables.count(var) > 0;
}

void Context::set(const std::string var, Result::num number) {
	if (variables.count(var) == 0) {
		variables.insert(make_pair(var, number));
	}
	else {
		variables[var] = number;
	}
}

Node* Node::parse(std::string str) {
	const int MAXCONST = (1 << 16) - 1;

	//Creating Reverse Polish notation
	std::vector<Token> stack, output;
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '(') {
			stack.push_back(Token(Token::Type::Bracket));
		}
		else if (str[i] == ')') {
			while (stack.size() > 0 && stack.back().type != Token::Type::Bracket) {
				output.push_back(stack.back());
				stack.pop_back();
			}
			if (stack.size() == 0) {
				return new ErrorNode("Incorrect bracket sequence");
			}
			stack.pop_back();
		}
		else if (isdigit(str[i])) {
			size_t len = 0;
			Token::num num = 0;
			while (i + len < str.length() && isdigit(str[i + len])) {
				num = num * 10 + str[i + len] - '0';
				len++;
				if (num > MAXCONST) {
					return new ErrorNode("Too big number");
				}
			}
			i += len - 1;
			output.push_back(Token(Token::Type::Number, num));
		}
		else if (isalpha(str[i])) {
			size_t len = 0;
			while (i + len < str.length() && isalnum(str[i + len])) {
				len++;
			}
			output.push_back(Token(Token::Type::Var, str.substr(i, len)));
			i += len - 1;
		}
		else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') {
			Token::Type op;
			if (str[i] == '+') {
				op = Token::Type::Add;
			}
			else if (str[i] == '-') {
				op = Token::Type::Sub;
			}
			else if (str[i] == '*') {
				op = Token::Type::Mul;
			}
			else if (str[i] == '/') {
				op = Token::Type::Div;
			}
			while (stack.size() > 0 && stack.back().priority() >= Token(op).priority()) {
				output.push_back(stack.back());
				stack.pop_back();
			}
			stack.push_back(Token(op));
		}
		else if (str[i] != ' ') {
			return new ErrorNode("Wrong symbol");
		}
	}
	while (stack.size() > 0) {
		if (!stack.back().isOp2()) {
			return new ErrorNode("Invalid expression");

		}
		output.push_back(stack.back());
		stack.pop_back();
	}

	//Creating AST
	std::vector<Node*> nodes;
	if (output.size() == 0) {
		return new ErrorNode("Empty expression");
	}
	for (const Token& token : output) {
		if (token.isOp2()) {
			if (nodes.size() < 2) {
				for (Node* node : nodes) {
					delete node;
				}
				return new ErrorNode("Wrong expression");
			}
			Node* r = nodes.back();
			nodes.pop_back();
			Node* l = nodes.back();
			nodes.pop_back();
			if (token.type == Token::Type::Add) {
				nodes.push_back(new AddNode(l, r));
			}
			else if (token.type == Token::Type::Sub) {
				nodes.push_back(new SubNode(l, r));
			}
			else if (token.type == Token::Type::Mul) {
				nodes.push_back(new MulNode(l, r));
			}
			else if (token.type == Token::Type::Div) {
				nodes.push_back(new DivNode(l, r));
			}
		}
		else if (token.type == Token::Type::Number) {
			nodes.push_back(new NumberNode(token.number));
		}
		else if (token.type == Token::Type::Var) {
			nodes.push_back(new VarNode(token.str));
		}
	}
	if (nodes.size() != 1) {
		return new ErrorNode("Wrong expression");
	}
	return nodes.back();
}


AST::AST() {
	root = new ErrorNode("Empty");
};

AST::~AST() {
	delete root;
};

void AST::swap(AST& ast) {
	std::swap(root, ast.root);
}

void AST::parse(const std::string str) {
	delete root;
	root = Node::parse(str);
};

Result AST::calculate(const Context& context) const {
	return root->calculate(context);
};

bool AST::error() const {
	return root->error();
}

std::string AST::errorMessage() const {
	if (error()) {
		return root->message;
	}
	else {
		return "No errors";
	}
}


