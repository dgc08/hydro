#include "include/generator.h"
#include "include/ASM_manager.h"
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
    push rbp
    mov rbp, rsp

%MAIN%
)";


std::string assembleTemplate(std::string main, std::string data, std::string text) {
  if (main == "") {
    main = "    mov eax, 0\n    ret";
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


AST& peek_ast(std::vector<AST>& contents, size_t pos, size_t peek) {
  if (pos+peek <= contents.size()) {
    return contents[pos+peek];
  }
}
void process_statement(AST& astree, ASM_manager& asm_m);
size_t expression_to_rax(AST& astree, ASM_manager& asm_m);

std::string assemble_from_ast(const AST astree) {
  ASM_manager asm_m;

  if ((astree.type() != NodeType::scope) && (astree.value() != "global")) {
    std::cout << "Code generator got passed an AST that isn't global scope" << std::endl;
    exit(1);
  }

  std::vector<AST> contents = astree.get_tree();
  for (size_t i = 0; i < astree.size(); i++) {
    AST& astree_item = contents[i];

    switch (astree_item.type()) {
      case NodeType::statement:
        process_statement(astree_item, asm_m);
        break;
      default:
        std::cout << "Can't parse anything else yet" << std::endl;
        exit(1);
    }
  }
  return assembleTemplate(asm_m.str_main(), asm_m.str_data(), asm_m.str_text());
}

void process_statement(AST& astree, ASM_manager& asm_m) {
  std::vector<AST> contents = astree.get_tree();
  for (size_t i = 0; i < astree.size(); i++) {
    AST& astree_item = contents[i];

    switch (astree_item.type()) {
      case NodeType::builtin_directive:
        if (astree_item.value() == "exit" && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          expression_to_rax(contents[i], asm_m);
          asm_m.add_instr_main("mov edi, eax");
          asm_m.add_instr_main("mov eax, 60");
          asm_m.add_instr_main("syscall");
        }
        else if (astree_item.value() == "return" && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          expression_to_rax(contents[i], asm_m);
          asm_m.add_instr_main("jmp _hy_main_ret");
        }
        else if (astree_item.value() == "let" && peek_ast(contents, i, 1).type() == NodeType::identifier && peek_ast(contents, i, 2).type() == NodeType::expression) {
          i++;
          std::string ident = contents[i].value();
          i++;

          size_t type = expression_to_rax(contents[i], asm_m);

          Mem_location loc = asm_m.define_local_var(ident, type);

          std::stringstream locale;
          locale << "mov [rsp+" << loc.offset << "], eax";
          asm_m.add_instr_main(locale.str());
        }
        else if (astree_item.value() == "print" && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          asm_m.add_instr_text("printf", "extern printf");

          size_t type = expression_to_rax(contents[i], asm_m);

          std::string fmt;
          switch (type) {
            case 4:
              fmt = "_hy_fmt_digit";
              asm_m.add_instr_data(fmt, fmt+"    db \"%d\", 10, 0");
              break;
            case 64:
              fmt = "_hy_fmt_digit_i64";
              asm_m.add_instr_data(fmt, fmt+"    db \"%lld\", 10, 0");
              break;
          }


          asm_m.add_instr_main("mov rsi, rax");
          asm_m.add_instr_main("mov rdi, _hy_fmt_digit");
          asm_m.add_instr_main("xor rax, rax");
          asm_m.add_instr_main("call printf wrt ..plt");
        }


        break;
      default:
        std::cout << "Can't parse anything else than just builtin directives as statements, got "<< astree_item.value() << std::endl;
        break;
    }
  }
}

size_t expression_to_rax(AST& astree, ASM_manager& asm_m) {
  std::vector<AST> contents = astree.get_tree();
  if (astree.size() == 1) {
    switch (contents[0].type()) {
    case NodeType::int_lit:
      asm_m.add_instr_main("mov eax, " + contents[0].value());
      return 4;
    case NodeType::identifier:
      {
        Mem_location loc = asm_m.get_local_var(contents[0].value());
        asm_m.add_instr_main("mov rax, [rsp + " + std::to_string(loc.offset) + "]");
        break;
      }
    default:
      std::cout << "Can't parse anything else than just one int into expr rn" << std::endl;
    }

  }
}
