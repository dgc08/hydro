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
  if (i >= tokens.size()) {
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
  tokens.clear();
}


void AST::parse_statement(std::vector<Token> tokens_arg) {
  setup_parsing (tokens_arg);

  _type = NodeType::statement;
  _value = token_p.value;

  AST directive;

  switch (token_p.type) {
    case TokenType::builtin_directive:
      directive.set_base_token(NodeType::builtin_directive, token_p.value);
      content.push_back(directive);
      if (token_p.value == "let") {
        next();
        if (token_p.type != TokenType::identifier) {
          std::cout << "Expected identifier not '" << token_p.value <<"'" << std::endl;
          exit(0);
        }
        AST identifier;
        identifier.set_base_token(NodeType::identifier, token_p.value);
        content.push_back(identifier);

        next();
        if (token_p.value != "=") {
          std::cout << "Expected '=' not '" << token_p.value <<"'" << std::endl;
        }
        next();
      }
      else {
        next();
      }
      break;
    case TokenType::eof:
      return;
    default:
      std::cout << "Can't parse anything else than just builtin directives as statements, got " << token_p.value << std::endl;
      break;
  }
  tokens.erase(tokens.begin(), tokens.begin() + i);


  AST expression;
  expression.parse_expression(tokens);
  tokens.clear();
  content.push_back(expression);
}


void AST::parse_expression(std::vector<Token> tokens_arg) {
  setup_parsing (tokens_arg);

  _type = NodeType::expression;
  _value = "lit";

  if (token_p.value == "(") {
    next();

    std::vector<Token> buf;
    int nestings = 1;
    while (nestings > 0) {
      buf.push_back(token_p);

      if (token_p.type == TokenType::eof) {
        std::cout << "No closing ) found, nestings: " << nestings << std::endl;
        exit(1);
      }
      else if  (token_p.value == "(") nestings++;

      next();
      if  (token_p.value == ")") nestings--;
    }

    {
      AST expression;
      expression.parse_expression(buf);
      content.push_back(expression);
    }
    buf.clear();

    next();

    if (token_p.type != TokenType::op) {
      if (token_p.type == TokenType::eof) return;
      else {std::cout << "Unexpected " << token_p.value << ", expected operator or eof" << std::endl ; exit(1);}
    }

    parse_operator();
  }
  else if (tokens.size() == 1) {
    switch (token_p.type) {
      case TokenType::int_lit:
        {
          AST lit;
          lit.set_base_token(NodeType::int_lit, token_p.value);
          content.push_back(lit);

          tokens.erase(tokens.begin(), tokens.begin() + i);
          break;
        }
      case TokenType::identifier:
        {
          AST lit;
          lit.set_base_token(NodeType::identifier, token_p.value);
          content.push_back(lit);

          tokens.erase(tokens.begin(), tokens.begin() + i);
        }
        break;
      default:
        std::cout << "Can't parse anything else than ints and identifiers into expr rn, got " << token_p.value  << std::endl;
    }
  }
  else {
    if (token_p.type == TokenType::int_lit || token_p.type == TokenType::identifier) {
      {
        AST expression;
        expression.parse_expression({token_p});
        content.push_back(expression);
      }
      next();

      if (token_p.type != TokenType::op) {
        std::cout << "Expected operator, got " << token_p.value  << std::endl;
        exit(1);
      }

      parse_operator();
    }
  }
}

void AST::parse_operator() {
    _value = token_p.value;

    next();
    if (i == tokens.size()) {
      std::cout << "Unexpected eof, expected expression" << std::endl ; exit(1);
    }
    tokens.erase(tokens.begin(), tokens.begin() + i);

    {
        AST expression;
        expression.parse_expression(tokens);
        content.push_back(expression);
    }
    tokens.clear();
}

void AST::set_base_token(NodeType type, std::string value) {
  _value = value;
  _type = type;
}
