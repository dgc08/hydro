#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <string>
#include <vector>

enum class TokenType
{
  _return,
  int_lit,
  semi,
  identifier
};

struct Token
{
  TokenType type;
  std::string value;
};

std::vector<Token> tokenize(const std::string& str);

#endif // TOKENIZER_H_
