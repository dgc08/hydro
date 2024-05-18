#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <vector>
#include <optional>

#include "tokenizer.h"

enum class NodeType {
  scope,
  statement,
  expression,

  builtin_directive,
  label,

  int_lit,

  identifier,
};

class AST {
  public:
    void parse_tokens(std::vector<Token> tokens_arg);

    void parse_expression(std::vector<Token> tokens_arg);
    void parse_statement(std::vector<Token> tokens_arg);
    void set_base_token(NodeType type, std::string value);

    std::vector<AST> get_tree() const {return content;}
    size_t size() const {return content.size();}

    std::string value() const {return _value;}
    NodeType type() const{return _type;}

    bool is_base_token() {return content.empty();}
    Token get_base_token();

  private:
    void setup_parsing(std::vector<Token> tokens_arg);
    inline void parse_operator();

    size_t i;
    std::vector<Token> tokens;
    Token token_p;
    Token next();

    std::vector<AST> content;
    NodeType _type;
    std::string _value;
};

#endif // PARSER_H_
