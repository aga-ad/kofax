#pragma once

#include <string>
#include <map>
#include <iostream>


struct Result {
	typedef long long num;
	num number;
	std::string message;

	Result(std::string message);
	Result(num number);
	bool error() const;
};

std::ostream& operator<<(std::ostream& os, const Result& res);


struct Token {
	//private:
	typedef Result::num num;
	enum class Type { Bracket, Error, Var, Number, Add, Sub, Mul, Div };
	Type type;
	union {
		std::string str;
		num number;
	};

	Token(const Token& token);
	Token(Type type);
	Token(Type type, std::string s);
	Token(Type type, num num);
	~Token();

	int priority() const;
	bool isOp2() const;

	//friend Node* Node::parse(std::string str);
};


class Context {
public:
	Result get(const std::string var) const;
	bool in(const std::string var) const;
	void set(const std::string var, Result::num number);
private:
	std::map<std::string, Result::num> variables;
};

class Node;

class AST {
public:
	AST();
	AST(const AST&) = delete;
	AST& operator=(const AST&) = delete;
	~AST();

	void swap(AST& ast);
	void parse(const std::string str);
	Result calculate(const Context& context) const;
	bool error() const;
	std::string errorMessage() const;
private:
	Node* root;
};


class Node {
protected:
	typedef Result::num num;
	union {
		Node* child[2];
		std::string message;
		std::string var;
		num number;
	};
	Node() {};

	static Node* parse(std::string str);

	friend class AST;
public:
	virtual ~Node() {};
	virtual Result calculate(const Context& context) const = 0;
	virtual bool error() const {
		return 0;
	}
};

class ErrorNode : public Node {
protected:
	ErrorNode(const std::string& message) {
		new (&this->message) std::string(message);
	};
	~ErrorNode() {
		message.~basic_string<char>();
	}
	Result calculate(const Context& context) const {
		return Result(message);
	};
	bool error() const {
		return 1;
	}
	friend Node* Node::parse(std::string str);
	friend class AST;
};

class NumberNode : public Node {
protected:
	NumberNode(num number) {
		this->number = number;
	};
	Result calculate(const Context& context) const {
		return Result(number);
	};
	friend Node* Node::parse(std::string str);
};

class VarNode : public Node {
protected:
	VarNode(std::string var) {
		new (&this->var) std::string(var);
	};
	Result calculate(const Context& context) const {
		return context.get(var);
	};
	~VarNode() {
		var.~basic_string<char>();
	}
	friend Node* Node::parse(std::string str);
};


class Op2Node : public Node {
protected:
	Op2Node(Node* l, Node* r) {
		child[0] = l;
		child[1] = r;
	};
	virtual ~Op2Node() {
		delete child[0];
		delete child[1];
	}
	Result calculate(const Context& context) const {
		Result l = child[0]->calculate(context);
		if (l.error()) {
			return l;
		}
		Result r = child[1]->calculate(context);
		if (r.error()) {
			return r;
		}
		return func(l.number, r.number);
	};

	virtual Result func(num l, num r) const = 0;

};

class AddNode : public Op2Node {
protected:
	AddNode(Node* l, Node* r) : Op2Node(l, r) {};
	Result func(Node::num l, Node::num r) const {
		if ((l > 0 && r > std::numeric_limits<Result::num>::max() - l) || \
			(l < 0 && r < std::numeric_limits<Result::num>::min() - l)) {
			return Result("Overflow");
		}
		return Result(l + r);
	}
	friend Node* Node::parse(std::string str);
};

class SubNode : public Op2Node {
protected:
	SubNode(Node* l, Node* r) : Op2Node(l, r) {};
	Result func(Node::num l, Node::num r) const {
		if (r == std::numeric_limits<Result::num>::min() || \
			(l > 0 && -r > std::numeric_limits<Result::num>::max() - l) || \
			(l < 0 && -r < std::numeric_limits<Result::num>::min() - l)) {
			return Result("Overflow");
		}
		return Result(l - r);
	}
	friend Node* Node::parse(std::string str);
};

class MulNode : public Op2Node {
protected:
	MulNode(Node* l, Node* r) : Op2Node(l, r) {};
	Result func(Node::num l, Node::num r) const {
		if (l != 0 && l * r / l != r) {
			return Result("Overflow");
		}
		return Result(l * r);
	}
	friend Node* Node::parse(std::string str);
};

class DivNode : public Op2Node {
protected:
	DivNode(Node* l, Node* r) : Op2Node(l, r) {};
	Result func(Node::num l, Node::num r) const {
		if (r == 0) {
			return Result("Division by zero");
		}
		return Result(l / r);
	}
	friend Node* Node::parse(std::string str);
};