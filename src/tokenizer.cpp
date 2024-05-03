#include "include/tokenizer.h"
#include "include/keywords.h"

#include <string>
#include <iostream>
#include <vector>

int i = 0;
char c;
void next(const std::string &str) {
  i++;
  if (i >= str.length()) {
    c = ' ';
  }
  else {
    c = str.at(i);
  }
}

std::vector<Token> tokenize(const std::string& str){
  std::vector<Token> tokens;
  std::string buf;

  c = str.at(0);
  while (i < str.length()) {
    Token token;

    // Identifiers & builtin_directives
    if (std::isalpha(c)) {
      while (std::isalnum(c)) {
          buf += c;
          next(str);
      }

      token.value = buf;

      token.type = check_keyword(buf);

    }
    // int_lit
    else if (std::isdigit(c)) {
      while (std::isdigit(c)) {
          buf += c;
          next(str);
      }
      token.value = buf;
      token.type = TokenType::int_lit;
    }
    // SEP
    else if (c == ';' || c == '=' || c == '(' || c == ')') {
      token.value = c;
      token.type = TokenType::sep;

      next(str);
    }
    // OP
    else if (c == '+' || c == '-' || c == '*' || c == '/') {
      token.value = c;
      token.type = TokenType::op;

      next(str);
    }
    else if (std::isspace(c)) {
      next(str);
      continue;
    }
    else {
      std::cout << "Error parsing '" << c << "' into token: Aborting";
      exit(1);
    }


    tokens.push_back(token);
    buf.clear();

  }
  tokens.push_back(Token{.type=TokenType::eof, .value=" "});
  return tokens;
}
