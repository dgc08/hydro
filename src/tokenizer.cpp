#include "include/tokenizer.h"

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

    if (std::isalpha(c)) {
      while (std::isalnum(c)) {
          buf += c;
          next(str);
      }

      token.value = buf;

      if (buf == "return") {
        token.type = TokenType::_return;
      }
      else {
        token.type = TokenType::identifier;
      }

    }
    else if (std::isdigit(c)) {
      while (std::isdigit(c)) {
          buf += c;
          next(str);
      }
      token.value = buf;
      token.type = TokenType::int_lit;
    }
    else if (c == ';') {
      token.value = ";";
      token.type = TokenType::semi;

      next(str);
    }
    else if (std::isspace(c)) {
      next(str);
      continue;
    }
    else {
      std::cout << "Error parsing '" << c << "': Aborting";
      exit(1);
    }


    tokens.push_back(token);
    buf.clear();

  }
  return tokens;
}
