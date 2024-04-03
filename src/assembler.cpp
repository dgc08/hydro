#include "include/assembler.h"

#include <string>
#include <sstream>

const char* assemblyTemplateString = R"(%DATA%
section .text
    global _start
    extern main

_start:
    call main       ; Call the extra main label
    mov edi, eax    ; mov return value from main
    mov rax, 60     ; syscall number for sys_exit
    syscall         ; invoke syscall

main:              ; main
%MAIN%
)";


std::string assembleTemplate(std::string main, std::string data) {
  if (main == "") {
    main = "    mov eax, 0\n    ret";
  }
  if (data != "") {
    data = "section .data\n" + data;
  }

  std::string asm_code(assemblyTemplateString);

  asm_code.replace(asm_code.find("%MAIN%"), sizeof("%MAIN%") - 1, main);
  asm_code.replace(asm_code.find("%DATA%"), sizeof("%DATA%") - 1, data);

  return asm_code;
}

std::string assemble_from_tokens(const std::vector<Token>& tokens) {
  std::stringstream main;
  std::stringstream data;

  for (int i = 0; i+1 < tokens.size(); i++) {
    const Token& token = tokens.at(i);
    if (token.type == TokenType::_return) {
      if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
        if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
          main << "    mov rax, 60\n";
          main << "    mov rdi, " << tokens.at(i + 1).value << "\n";
          main << "    syscall";

          i+= 2;
        }
      }
    }
  }

  return assembleTemplate(main.str(), data.str());
}
