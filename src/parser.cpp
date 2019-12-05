#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct token {
  enum Type {
    integer,
    plus,
    times,
    par_open,
    par_close,
  };

  Type type;

  int number;

  token(Type type, int number = 0)
          :type{ type }, number{ number } {
  }

  std::string format() const {
    switch (type) {
    case token::integer:
      return std::to_string(number);
      break;
    case token::plus:
      return "+";
    case token::times:
      return "*";
    case token::par_open:
      return "(";
    case token::par_close:
      return ")";
    default:
      return "<invalid>";
    }
  }
};

std::vector<token> tokenize(std::istream& in) {
  std::vector<token> res;
  while (in) {
    int peek = in.peek();
    if (peek < 0)
      break;
    if (isspace(peek)) {
      in.get();
      continue;
    }
    if (peek >= '0' && peek <= '9') {
      int tmp;
      in >> tmp;
      res.emplace_back(token::integer, tmp);
      continue;
    }
    switch (peek) {
    case '(':
      res.emplace_back(token::par_open);
      break;
    case ')':
      res.emplace_back(token::par_close);
      break;
    case '+':
      res.emplace_back(token::plus);
      break;
    case '*':
      res.emplace_back(token::times);
      break;
    default:
      std::cerr << "I do not recognize the token: " << peek
                << std::endl;
    }
    in.get();
  }
  return res;
}
class Expr {
public:
  virtual int eval() = 0;
  virtual std::string print() = 0;
};

class Num : public Expr {
  int number;

public:
  Num(int number)
          :number(number) {
  }
  int eval() override {
    return number;
  };
  std::string print() override {
    return std::to_string(number);
  }
};

class BinaryOper : public Expr {
public:
  BinaryOper(std::unique_ptr<Expr>&& a, std::unique_ptr<Expr>&& b)
          :left(std::move(a)), right(std::move(b)) {
  }

protected:
  std::unique_ptr<Expr> left, right;
};

class Add : public BinaryOper {
public:
  Add(std::unique_ptr<Expr>&& a, std::unique_ptr<Expr>&& b)
          :BinaryOper(std::move(a), std::move(b)) {
  }
  int eval() override {
    return left->eval() + right->eval();
  }

  std::string print() override {
    return "(" + left->print() + " + " + right->print() + ")";
  }
};

class Multiply : public BinaryOper {
public:
  Multiply(std::unique_ptr<Expr>&& a, std::unique_ptr<Expr>&& b)
          :BinaryOper(std::move(a), std::move(b)) {
  }
  int eval() override {
    return left->eval() * right->eval();
  }

  std::string print() override {
    return "(" + left->print() + " * " + right->print() + ")";
  }
};

using Pos = std::vector<token>::iterator;

std::unique_ptr<Expr> parseSimpleExpr(Pos& begin, Pos end);

std::unique_ptr<Expr> parseMultExpr(Pos& begin, Pos end) {
  auto l = parseSimpleExpr(begin, end);
  if (!l || begin == end || begin->type != token::times) return l;
  begin++;
  auto r = parseMultExpr(begin, end);
  if (!r) {
    begin--;
    return l;
  }
  return std::make_unique<Multiply>(std::move(l), std::move(r));
}

std::unique_ptr<Expr> parseAddExpr(Pos& begin, Pos end) {
  auto l = parseMultExpr(begin, end);
  if (!l || begin == end || begin->type != token::plus)
    return l;
  begin++;
  auto r = parseAddExpr(begin, end);
  if (!r) {
    begin--;
    return l;
  }
  return std::make_unique<Add>(std::move(l), std::move(r));
}

std::unique_ptr<Expr> parseExpr(Pos& begin, Pos end) {
  return parseAddExpr(begin, end);
}

std::unique_ptr<Expr> parseSimpleExpr(Pos& begin, Pos end) {
  if (begin == end) return nullptr;
  if (begin->type == token::integer) {
    int num = begin->number;
    begin++;
    return std::make_unique<Num>(num);
  }
  if (begin->type == token::par_open) {
    auto begin_bck = begin;
    begin++;
    auto e = parseExpr(begin, end);
    if (!e) {
      begin--;
      return nullptr;
    }
    if (begin->type == token::par_close) {
      begin++;
      return e;
    }
    begin = begin_bck;
  }

  return nullptr;
}
