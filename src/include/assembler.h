#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <string>
#include <vector>

#include "tokenizer.h"

std::string assembleTemplate(std::string main = "", std::string data = "");
std::string assemble_from_tokens(const std::vector<Token>& tokens);

#endif // ASSEMBLER_H_
