#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <vector>
#include <optional>

#include "tokenizer.h"

enum class NodeType {
  scope,
  builtin_directive,
  expression,
  identifier,
  seperator
};

struct AST_Base_Node {};


class AST {
  public:
    static AST parse_tokens(std::vector<Token>);
    std::vector<AST> get_tree();
    bool is_base_token();
    std::optional<Token> get_base_token();
  private:
    size_t i;

};

#endif // PARSER_H_
