#include <iostream>

#include <string>
#include <vector>
#include <optional>

#include "include/parser.h"
#include "include/tokenizer.h"

Token AST::get_base_token() {
  Token token;
  if (is_base_token()) {
    token.value = _value;
    switch (_type) {
      case NodeType::builtin_directive: token.type = TokenType::builtin_directive; break;
      case NodeType::int_lit: token.type = TokenType::int_lit; break;

      default: std::cout << "Requested base token for unkown TokenType" ; exit(1);
    }

  }

  return token;
}


Token AST::next() {
  i++;
  if (i > tokens.size()) {
    token_p = Token{.type=TokenType::eof, .value=" "};
  }
  else {
    token_p = tokens[i];
  }

  return token_p;
}


void AST::setup_parsing(std::vector<Token> tokens_arg) {
  tokens = tokens_arg;

  i = 0;
  if (tokens.size() > 0) {
     token_p = tokens[i];
  }
}

void AST::parse_tokens(std::vector<Token> tokens_arg) {
  setup_parsing (tokens_arg);

  _type = NodeType::scope;
  _value = "global";
  std::vector<Token> buf;

  while (token_p.type != TokenType::eof) {
    switch (token_p.type) {

      // Case: statement
      case TokenType::builtin_directive:
      case TokenType::identifier:
        {
          // Statment
          while (token_p.value != ";") {
            buf.push_back(token_p);
            next();
          }
          next(); // Conusme the ;
          AST statement;
          statement.parse_statement(buf);
          buf.clear();
          content.push_back(statement);
          break;
        }

      default:
        std::cout << "Can't parse anything else than just directives (theoretically but no actually identifiers) as top level tokens" << std::endl;
        exit(1);
    }
  }
}


void AST::parse_statement(std::vector<Token> tokens_arg) {
  setup_parsing (tokens_arg);

  _type = NodeType::statement;
  _value = token_p.value;

  AST directive;

  switch (token_p.type) {
    case TokenType::builtin_directive:
      directive.set_base_token(NodeType::builtin_directive, token_p.value);
      break;
    case TokenType::identifier:
      std::cout << "Can't parse anything else than just builtin directives as statements" << std::endl;
      break;
  }
  content.push_back(directive);
  tokens.erase(tokens.begin());


  AST expression;
  expression.parse_expression(tokens);
  tokens.clear();
  content.push_back(expression);
}


void AST::parse_expression(std::vector<Token> tokens_arg) {
  setup_parsing (tokens_arg);

  _type = NodeType::expression;
  _value = token_p.value;

  switch (token_p.type) {
    case TokenType::int_lit:
      if (tokens.size() == 1) {
        AST lit;
        lit.set_base_token(NodeType::int_lit, token_p.value);
        content.push_back(lit);

        tokens.clear();
        break;
      }
    default:
      std::cout << "Can't parse anything else than just one int into expr rn" << std::endl;
  }
}

void AST::set_base_token(NodeType type, std::string value) {
  _value = value;
  _type = type;
}
