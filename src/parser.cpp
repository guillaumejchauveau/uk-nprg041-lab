
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

/*
 * tokenizer
 */

enum class token_type { integer, identifier, single_char };

struct Token {
  token_type type;
  int value;
  string name;
  int position;

  Token(int position, token_type type, int value = 0) {
    this->position = position;
    this->type = type;
    this->value = value;
  }
  Token(int position, string &&name) {
    this->position = position;
    this->type = token_type::identifier;
    this->value = 0;
    this->name = name;
  }
  string format() const {
    switch (type) {
    case token_type::integer:
      return to_string(value);
    case token_type::identifier:
      return name;
    case token_type::single_char:
      return string(1, this->value);
    default:
      return "<unknown_token>";
    }
  }

  bool operator==(token_type type) { return this->type == type; }
  bool operator==(const char &value) { return this->value == value; }
  bool operator!=(token_type type) { return this->type != type; }
  bool operator!=(const char &value) { return this->value != value; }
};

class SyntaxError : public runtime_error {
  Token token;

public:
  explicit SyntaxError(Token token)
      : runtime_error("Syntax error near token '" + token.format() +
                      "' at position " + to_string(token.position)),
        token(move(token)) {}
};

vector<Token> tokenize(istream &in) {
  char c;
  vector<Token> res;
  int pos = 0;
  while (in) {
    c = in.peek();
    if (c < 0) {
      continue;
    }
    if (c == '.') {
      // use instead of EOF
      pos++;
      in.get();
      break;
    }
    if (isspace(c)) {
      pos++;
      in.get();
      continue;
    }
    if (isdigit(c)) {
      int n;
      in >> n;
      res.emplace_back(pos, token_type::integer, n);
      continue;
    }
    if (islower(c)) {
      string s;
      int start_pos = pos;
      while (islower(in.peek())) {
        pos++;
        s += static_cast<char>(in.get());
      }
      res.emplace_back(start_pos, move(s));
      continue;
    }

    res.emplace_back(pos, token_type::single_char, c);
    // cerr << "Unexpected character: '" << c << "' " << int(c) << endl;
    pos++;
    in.get();
  }
  return res;
}

/*
 * interfaces for AST
 * (very abstract now)
 */

class Expression {
public:
  virtual string toString() = 0;
  virtual size_t outputProgram(ostream &o) const = 0;
};

class Program {
public:
  virtual string toString() = 0;
  virtual size_t outputProgram(ostream &o) const = 0;
};

/*
 * program implementations
 */
class Statement : public Program {
protected:
  string identifier;
  unique_ptr<Expression> expression;
  unique_ptr<Program> program;

public:
  explicit Statement(string identifier) : identifier(move(identifier)) {}
  Statement(string identifier, unique_ptr<Expression> &&expression)
      : identifier(move(identifier)), expression(move(expression)) {}
  Statement(string identifier, unique_ptr<Program> &&statement)
      : identifier(move(identifier)), program(move(statement)) {}
};

class StatementGroup : public Program {
protected:
  list<unique_ptr<Program>> statements;

public:
  void addStatement(unique_ptr<Program> &&statement) {
    this->statements.push_back(move(statement));
  }
  string toString() override {
    string render = "{";
    for (const auto &statement : this->statements) {
      render += "\n  " + statement->toString() + ";";
    }
    if (render.back() == ';') {
      render.pop_back();
    }
    return render + "\n}";
  }

  size_t outputProgram(ostream &o) const override {
    size_t size = 0;
    for (const auto &statement : this->statements) {
      size += statement->outputProgram(o);
    }
    return size;
  }
};

class Out : public Statement {
public:
  explicit Out(string identifier) : Statement(move(identifier)) {}
  string toString() override { return "< " + this->identifier; }

  size_t outputProgram(ostream &o) const override {
    o << "LOADVAR " << this->identifier << endl;
    o << "WRITE" << endl;
    return 2;
  }
};

class In : public Statement {
public:
  explicit In(string identifier) : Statement(move(identifier)) {}
  string toString() override { return "> " + this->identifier; }

  size_t outputProgram(ostream &o) const override {
    o << "READ" << endl;
    o << "STOREVAR " << this->identifier << endl;
    return 2;
  }
};

class Assign : public Statement {
public:
  Assign(string identifier, unique_ptr<Expression> &&expression)
      : Statement(move(identifier), move(expression)) {}
  string toString() override {
    return "= " + this->identifier + " " + this->expression->toString();
  }

  size_t outputProgram(ostream &o) const override {
    size_t size = this->expression->outputProgram(o);
    o << "STOREVAR " << this->identifier << endl;
    return size + 1;
  }
};

class While : public Statement {
public:
  While(string identifier, unique_ptr<Program> &&program)
      : Statement(move(identifier), move(program)) {}
  string toString() override {
    return "@ " + this->identifier + " " + this->program->toString();
  }

  size_t outputProgram(ostream &o) const override {
    ostringstream subStream;
    size_t size = this->program->outputProgram(subStream);
    o << "LOADVAR " << this->identifier << endl;
    o << "JMPF " << size + 2 << endl;
    o << subStream.str();
    o << "JMP " << -static_cast<int>(size) - 2 << endl;
    return 2 + size + 1;
  }
};

class Conditional : public Statement {
public:
  Conditional(string identifier, unique_ptr<Program> &&program)
      : Statement(move(identifier), move(program)) {}
  string toString() override {
    return "? " + this->identifier + " " + this->program->toString();
  }

  size_t outputProgram(ostream &o) const override {
    ostringstream subStream;
    size_t size = this->program->outputProgram(subStream);
    o << "LOADVAR " << this->identifier << endl;
    o << "JMPF " << size + 1 << endl;
    o << subStream.str();
    return 2 + size;
  }
};

class NConditional : public Statement {
public:
  NConditional(string identifier, unique_ptr<Program> &&program)
      : Statement(move(identifier), move(program)) {}
  string toString() override {
    return "! " + this->identifier + " " + this->program->toString();
  }

  size_t outputProgram(ostream &o) const override {
    ostringstream subStream;
    size_t size = this->program->outputProgram(subStream);
    o << "LOADVAR " << this->identifier << endl;
    o << "JMPT " << size + 1 << endl;
    o << subStream.str();
    return 2 + size;
  }
};

/*
 * expressions
 */

class Number : public Expression {
  int val;

public:
  explicit Number(int v) : val(v) {}

  string toString() override { return ::to_string(this->val); }
  size_t outputProgram(ostream &o) const override {
    o << "INT " << this->val << endl;
    return 1;
  }
};

class Variable : public Expression {
  string name;

public:
  explicit Variable(string v) : name(std::move(v)) {}
  string toString() override { return this->name; }
  size_t outputProgram(ostream &o) const override {
    o << "LOADVAR " << this->name << endl;
    return 1;
  }
};

enum operator_type { add = '+', subtract = '-', multiply = '*' };

class Operator : public Expression {
  operator_type type;
  unique_ptr<Expression> left, right;

public:
  Operator(operator_type type, unique_ptr<Expression> &&left,
           unique_ptr<Expression> &&right)
      : type(type), left(move(left)), right(move(right)) {}
  string toString() override {
    return "(" + this->left->toString() + static_cast<char>(this->type) +
           this->right->toString() + ")";
  }

  size_t outputProgram(ostream &o) const override {
    size_t size = this->left->outputProgram(o);
    size += this->right->outputProgram(o);
    switch (type) {
    case add:
      o << "ADD";
      break;
    case subtract:
      o << "SUB";
      break;
    case multiply:
      o << "MULT";
      break;
    }
    o << endl;
    return size + 1;
  }
};

/*
 * Parsing
 */

using Pos = vector<Token>::iterator;

unique_ptr<Expression> ParseExpr(Pos &begin, Pos end);

unique_ptr<Expression> ParseSimpleExpr(Pos &begin, Pos end) {
  if (begin == end)
    throw SyntaxError(*begin);
  if (*begin == token_type::integer) {
    unique_ptr<Expression> val = make_unique<Number>(begin->value);
    begin++;
    return val;
  }

  if (*begin == token_type::identifier) {
    unique_ptr<Expression> val = make_unique<Variable>(begin->name);
    begin++;
    return val;
  }

  if (*begin == '(') {
    auto original_begin = begin;
    begin++;

    unique_ptr<Expression> e = ParseExpr(begin, end);
    if (!e || begin == end || *begin != ')') {
      begin = original_begin;
      throw SyntaxError(*begin);
    }

    begin++;
    return e;
  }

  throw SyntaxError(*begin);
}

unique_ptr<Expression> ParseMulExpr(Pos &begin, Pos end) {
  unique_ptr<Expression> l = ParseSimpleExpr(begin, end);
  if (!l)
    throw SyntaxError(*begin);
  if (begin == end)
    return l;
  if (*begin != '*')
    return l;
  begin++;
  unique_ptr<Expression> r = ParseMulExpr(begin, end);
  if (!r) {
    begin--;
    return l;
  }
  return make_unique<Operator>(operator_type::multiply, move(l), move(r));
}

unique_ptr<Expression> ParseAddExpr(Pos &begin, Pos end) {
  unique_ptr<Expression> l = ParseMulExpr(begin, end);
  if (!l)
    return l;
  if (begin == end)
    return l;
  auto op = *begin;
  if (op != operator_type::add && op != operator_type::subtract)
    return l;
  begin++;
  unique_ptr<Expression> r = ParseAddExpr(begin, end);
  if (!r) {
    begin--;
    return l;
  }
  if (op == operator_type::add) {
    return make_unique<Operator>(operator_type::add, move(l), move(r));
  }
  return make_unique<Operator>(operator_type::subtract, move(l), move(r));
}

unique_ptr<Expression> ParseExpr(Pos &begin, Pos end) {
  return ParseAddExpr(begin, end);
}

unique_ptr<Program> ParseStatementGroup(Pos &begin, Pos end);

unique_ptr<Program> ParseStatement(Pos &begin, Pos end) {
  if (begin == end || *begin != token_type::single_char) {
    throw SyntaxError(*begin);
  }
  auto original_begin = begin;
  char statement_type = static_cast<char>(begin->value);
  begin++;
  if (begin == end || *begin != token_type::identifier) {
    begin = original_begin;
    throw SyntaxError(*begin);
  }

  string identifier = begin->name;
  unique_ptr<Expression> expression;
  unique_ptr<Program> program;

  begin++;

  switch (statement_type) {
  case '<':
    return make_unique<Out>(identifier);
  case '>':
    return make_unique<In>(identifier);
  case '=':
    expression = ParseExpr(begin, end);
    if (!expression) {
      begin = original_begin;
      throw SyntaxError(*begin);
    }
    return make_unique<Assign>(identifier, move(expression));
  case '@':
  case '?':
  case '!':
    program = ParseStatementGroup(begin, end);
    if (!program) {
      begin = original_begin;
      throw SyntaxError(*begin);
    }
    switch (statement_type) {
    case '@':
      return make_unique<While>(identifier, move(program));
    case '?':
      return make_unique<Conditional>(identifier, move(program));
    case '!':
      return make_unique<NConditional>(identifier, move(program));
    }
  default:
    throw SyntaxError(*begin);
  }
}

unique_ptr<Program> ParseStatementGroup(Pos &begin, Pos end) {
  if (begin == end) {
    throw SyntaxError(*begin);
  }
  auto original_begin = begin;

  if (*begin == '{') {
    begin++;
  }

  auto group = make_unique<StatementGroup>();

  while (*begin != '}' && begin != end) {
    auto statement = ParseStatement(begin, end);
    if (*begin == ';') {
      begin++;
    }
    group->addStatement(move(statement));
  }
  if (*begin == '}') {
    begin++;
  }
  return group;
}

/*
 * the main program
 */

int main() {
  try {
    auto tokens = tokenize(cin);
    auto b = tokens.begin();
    auto e = ParseStatementGroup(b, tokens.end());
    e->outputProgram(cout);
    cout << "QUIT";
  } catch (SyntaxError &e) {
    cout << "FAIL";
  }
  return 0;
}
