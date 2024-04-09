#include "include/keywords.h"

#include <algorithm>
#include <vector>
#include <string>

const std::vector<std::string> KEYWORDS_DIRECTIVE = {"exit", "return", "print", "let"};


bool in_vec (const std::vector<std::string>& vec, std::string target) {
  return (std::find(vec.begin(), vec.end(), target)) != vec.end();
}

TokenType check_keyword(std::string buf) {
  if (in_vec(KEYWORDS_DIRECTIVE, buf)) {
    return TokenType::builtin_directive;
  }
  else {
    return TokenType::identifier;
  }
}
