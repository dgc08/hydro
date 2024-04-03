#include <iostream>
#include <fstream>
#include <sstream>

#include "include/assembler.h"
#include "include/tokenizer.h"


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

  {
    std::fstream file(build_dir + "/out.asm", std::ios::out);
    file << assemble_from_tokens(tokens);
  }

  system(("nasm -felf64 " + build_dir + "/out.asm").c_str());
  system(("ld -o " + build_dir + "/out " + build_dir + "/out.o").c_str());

  return 0;
}
