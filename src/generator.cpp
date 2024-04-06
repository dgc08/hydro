#include "include/generator.h"
#include "include/section_stream.h"
#include "include/keywords.h"

#include <string>
#include <sstream>
#include <iostream>

const char* assemblyTemplateString = R"(%DATA%
section .text
    extern main
%TEXT%

main:
    call _hy_main       ; Call the extra main label

    mov edi, eax    ; mov return value from main
    mov rax, 60     ; syscall number for sys_exit
    syscall         ; invoke syscall

_hy_main:              ; main
%MAIN%
)";


std::string assembleTemplate(std::string main, std::string data, std::string text) {
  if (main == "") {
    main = "    mov eax, 0\n    ret";
  }
  else {
    main = "    mov rbp, rsp\n" + main;
  }

  if (data != "") {
    data = "section .data\n" + data;
  }

  std::string asm_code(assemblyTemplateString);

  asm_code.replace(asm_code.find("%MAIN%"), sizeof("%MAIN%") - 1, main);
  asm_code.replace(asm_code.find("%DATA%"), sizeof("%DATA%") - 1, data);
  asm_code.replace(asm_code.find("%TEXT%"), sizeof("%TEXT%") - 1, text);

  return asm_code;
}


void add_instr(std::stringstream& stream, std::string data) {
  stream << "    " << data << "\n";
}


AST& peek_ast(std::vector<AST>& contents, size_t pos, size_t peek) {
  if (pos+peek <= contents.size()) {
    return contents[pos+peek];
  }
}
void process_statement(AST& astree, std::stringstream& main, Section_Stream& data, Section_Stream& text);
DataType expression_to_rax(AST& astree, std::stringstream& main, Section_Stream& data, Section_Stream& text);

std::string assemble_from_ast(const AST astree) {
  std::stringstream main;
  Section_Stream data;
  Section_Stream text;

  if ((astree.type() != NodeType::scope) && (astree.value() != "global")) {
    std::cout << "Code generator got passed an AST that isn't global scope" << std::endl;
    exit(1);
  }

  std::vector<AST> contents = astree.get_tree();
  for (size_t i = 0; i < astree.size(); i++) {
    AST& astree_item = contents[i];

    switch (astree_item.type()) {
      case NodeType::statement:
        process_statement(astree_item, main, data, text);
        break;
      default:
        std::cout << "Can't parse anything else yet" << std::endl;
        exit(1);
    }
  }
  return assembleTemplate(main.str(), data.str(), text.str());
}

void process_statement(AST& astree, std::stringstream& main, Section_Stream& data, Section_Stream& text) {
  std::vector<AST> contents = astree.get_tree();
  for (size_t i = 0; i < astree.size(); i++) {
    AST& astree_item = contents[i];

    switch (astree_item.type()) {
      case NodeType::builtin_directive:
        if (astree_item.value() == "exit" && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          expression_to_rax(contents[i], main, data, text);
          add_instr(main, "mov edi, eax");
          add_instr(main, "mov eax, 60");
          add_instr(main, "syscall");
        }
        else if (astree_item.value() == "return" && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          expression_to_rax(contents[i], main, data, text);
          add_instr(main, "pop rbp");
          add_instr(main, "ret");
        }
        else if (astree_item.value() == "print" && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          text.add_instr("extern printf", "extern printf");

          DataType type = expression_to_rax(contents[i], main, data, text);

          std::string fmt;
          switch (type) {
            case DataType::i32:
              fmt = "_hy_fmt_digit";
              data.add_instr(fmt, fmt+"    db \"%d\", 10, 0");
              break;
            case DataType::i64:
              fmt = "_hy_fmt_digit_i64";
              data.add_instr(fmt, fmt+"    db \"%lld\", 10, 0");
              break;
          }


          add_instr(main, "mov rsi, rax");
          add_instr(main, "mov rdi, _hy_fmt_digit");
          add_instr(main, "xor rax, rax");
          add_instr(main, "call printf wrt ..plt");
        }


        break;
      default:
        std::cout << "Can't parse anything else than just builtin directives as statements" << std::endl;
        break;
    }
  }
}

DataType expression_to_rax(AST& astree, std::stringstream& main, Section_Stream& data, Section_Stream& text) {
  std::vector<AST> contents = astree.get_tree();
  if (astree.size() == 1) {
    switch (contents[0].type()) {
    case NodeType::int_lit:
      add_instr(main, "mov eax, " + contents[0].value());
      return DataType::i32;
    default:
      std::cout << "Can't parse anything else than just one int into expr rn" << std::endl;
    }

  }
}
