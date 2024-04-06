#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <string>
#include <vector>

#include "parser.h"

std::string assembleTemplate(std::string main = "", std::string data = "");
std::string assemble_from_ast(const AST astree);

#endif // GENERATOR_H_
