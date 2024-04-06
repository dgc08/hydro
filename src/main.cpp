#include <iostream>
#include <fstream>
#include <sstream>

#include "include/generator.h"
#include "include/tokenizer.h"
#include "include/parser.h"


auto main(int argc, char** argv) -> int {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " filename" << std::endl;
    exit(1);
  }

  std::string build_dir=".";
  if (argc > 2) {
    build_dir = argv[2];
  }

  std::string source_code;
  {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        source_code = contents_stream.str();
  }

  std::vector<Token> tokens = tokenize(source_code);

  for (Token t : tokens) {
    /*
    if (t.type == TokenType::int_lit) std::cout << "Int lit @ "<< t.value << std::endl;
    if (t.type == TokenType::builtin_directive) std::cout << "return @"<< t.value << std::endl;
    if (t.type == TokenType::sep) std::cout << "semi @ "<< t.value << std::endl;
    if (t.type == TokenType::identifier) std::cout << "idnt @ "<< t.value << std::endl;
    */
  }

  AST astree;
  astree.parse_tokens(tokens);
  tokens.clear();
  {
    std::fstream file(build_dir + "/out.asm", std::ios::out);
    file << assemble_from_ast(astree);
  }

  system(("nasm -felf64 " + build_dir + "/out.asm").c_str());
  system(("gcc -no-pie -o " + build_dir + "/out " + build_dir + "/out.o").c_str());

  return 0;
}
