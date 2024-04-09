#ifndef KEYWORDS_H_s
#define KEYWORDS_H_s

#include "tokenizer.h"

#include <vector>
#include <string>

bool in_vec (const std::vector<std::string>& vec, std::string target);

TokenType check_keyword(std::string buf);


#endif // KEYWORDS_H_
