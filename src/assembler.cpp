#include "include/assembler.h"

#include <string>
#include <sstream>
#include <iostream>

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

Token seek(const std::vector<Token>& tokens, int pos, int seek) {
  if (pos + seek < tokens.size()) {
    return tokens.at(pos+seek);
  }
}

void add_instr(std::stringstream& stream, std::string data) {
  stream << "   " << data << "\n";
}

std::string assemble_from_tokens(const std::vector<Token>& tokens) {
  std::stringstream main;
  std::stringstream data;

  for (int i = 0; i+1 < tokens.size(); i++) {
    const Token& token = tokens.at(i);
    /* OLD test assembler
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
    */
    switch (token.type) {
    case TokenType::_return:
      if (seek(tokens, i, 1).type == TokenType::int_lit && seek(tokens, i, 2).type == TokenType::sep && seek(tokens, i, 2).value == ";") {
        add_instr(main, "mov rax, " + seek(tokens, i, 1).value);
        add_instr(main, "ret");
      }
      break;
    case TokenType::_exit:
      if (seek(tokens, i, 1).type == TokenType::int_lit && seek(tokens, i, 2).type == TokenType::sep && seek(tokens, i, 2).value == ";") {
        add_instr(main, "mov rax, 60");
        add_instr(main, "mov rdi, " + seek(tokens, i, 1).value);
        add_instr(main, "syscall");
      }
      break;
    }

  }


  return assembleTemplate(main.str(), data.str());
}
