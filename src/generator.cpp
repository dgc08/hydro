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
    push rsi
    push rdi
    call _hy_main

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

void process_scope(AST astree, ASM_manager& asm_m);
void process_statement(AST& astree, ASM_manager& asm_m);
size_t expression_to_rax(AST& astree, ASM_manager& asm_m);

std::string assemble_from_ast(const AST astree) {
  ASM_manager asm_m;
  asm_m.return_from_scope = true;

  if ((astree.type() != NodeType::scope) && (astree.value() != "global")) {
    std::cout << "Code generator got passed an AST that isn't global scope" << std::endl;
    exit(1);
  }

  process_scope(astree, asm_m);

  return assembleTemplate(asm_m.str_main(), asm_m.str_data(), asm_m.str_text());
}

void process_scope(AST astree, ASM_manager& asm_m) {
  std::vector<AST> contents = astree.get_tree();
  for (size_t i = 0; i < astree.size(); i++) {
    AST& astree_item = contents[i];
    switch (astree_item.type()) {
      case NodeType::statement:
        process_statement(astree_item, asm_m);
        break;
      case NodeType::label:
        i++;
        asm_m.add_instr_main(astree_item.value() + ":");
        astree_item = contents[i];
        break;
      default:
        std::cout << "Can't generate anything else than statements yet (process_scope), got " <<  astree_item.value() << std::endl;
        exit(1);
    }
  }
}

void process_statement(AST& astree, ASM_manager& asm_m) {
  std::vector<AST> contents = astree.get_tree();
  for (size_t i = 0; i < astree.size(); i++) {
    AST& astree_item = contents[i];

    switch (astree_item.type()) {
      case NodeType::builtin_directive:
        // exit directive implementation
        if (astree_item.value() == "exit" && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          expression_to_rax(contents[i], asm_m);
          asm_m.add_instr_main("mov edi, eax");
          asm_m.add_instr_main("mov eax, 60");
          asm_m.add_instr_main("syscall");
        }
        // return directive implementation
        else if ((astree_item.value() == "return")  && peek_ast(contents, i, 1).type() == NodeType::expression) {
          i++;
          expression_to_rax(contents[i], asm_m);
          asm_m.add_instr_main("jmp _hy_main_ret");
        }
        // call directive implementation
        else if ((astree_item.value() == "call" && peek_ast(contents, i, 1).type() == NodeType::expression)) {
          i++;
          asm_m.add_instr_main("call _hy_user_" + contents[i].get_tree()[0].value());
        }
        // end directive implementation
        else if ((astree_item.value() == "end")) {
          // if (peek_ast(contents, i, 1).type() == NodeType::expression) expression_to_rax(contents[i], asm_m);
          i++;
          asm_m.add_instr_main("ret");
        }
        // let directive implementation
        else if (astree_item.value() == "let" && peek_ast(contents, i, 1).type() == NodeType::identifier && peek_ast(contents, i, 2).type() == NodeType::expression) {
          i++;
          std::string ident = contents[i].value();
          i++;

          size_t type = expression_to_rax(contents[i], asm_m);

          Mem_location loc = asm_m.define_local_var(ident, type);

          std::string word_type = "Undefined please nasm throw an error";

          switch (type) {
            case 4:
              word_type = "";
              break;
          case 8:
              word_type = "QWORD";
              break;
          }

          std::stringstream locale;
          locale << "mov " << word_type << " [rbp-" << loc.offset+loc.size << "], eax";
          asm_m.add_instr_main(locale.str());
        }
        // let directive implementation
        else if (astree_item.value() == "set" && peek_ast(contents, i, 1).type() == NodeType::identifier && peek_ast(contents, i, 2).type() == NodeType::expression) {
          i++;
          std::string ident = contents[i].value();
          i++;

          size_t type = expression_to_rax(contents[i], asm_m);

          Mem_location loc = asm_m.get_local_var(ident);

          std::string word_type = "Undefined please nasm throw an error";

          switch (type) {
            case 4:
              word_type = "";
              break;
          case 8:
              word_type = "QWORD";
              break;
          }

          std::stringstream locale;
          locale << "mov " << word_type << " [rbp-" << loc.offset+loc.size << "], eax";
          asm_m.add_instr_main(locale.str());
        }
        // print directive implementation
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
            case 8:
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
        std::cout << "Can't generate anything else than just builtin directives as statements, got "<< astree_item.value() << std::endl;
        break;
    }
  }
}

size_t expression_to_rax(AST& astree, ASM_manager& asm_m) {
  std::vector<AST> contents = astree.get_tree();
  if (astree.value() == "lit") {
    switch (contents[0].type()) {
    case NodeType::int_lit:
      asm_m.add_instr_main("mov eax, " + contents[0].value());
      return 4;
    case NodeType::identifier:
      {
        if (contents[0].value()[0] == '$') {
          if (std::isalpha(contents[0].value()[1])) {
            asm_m.add_instr_main("mov rax, " + contents[0].value().substr(1));
            return 8;
          }
          else if (std::isdigit(contents[0].value()[1])) {
            int offset = 24 + std::stoi(contents[0].value().substr(1)) * 8;
            asm_m.add_instr_main("mov rax, [rbp+" + std::to_string(offset) + "]");
            return 8;
          }
        }

        Mem_location loc = asm_m.get_local_var(contents[0].value());
        std::string word_type = "Undefined please nasm throw an error";
        std::string reg = "Undefined please nasm throw an error";

          switch (loc.size) {
              case 4:
                  word_type = "";
                  reg = "eax";
                  //asm_m.add_instr_main("xor rax, rax");
                  break;
              case 8:
                word_type = "QWORD";
                reg = "rax";
                break;
          }
        asm_m.add_instr_main("mov " + word_type + " " + reg + ", [rbp-" + std::to_string(loc.offset+loc.size) + "]");
        return loc.size;
      }
    case NodeType::expression:
      return expression_to_rax(contents[0], asm_m);
    default:
      std::cout << "Got invalid lit" << std::endl;
    }
  }
  else if (astree.value() == "+" && astree.size() == 2) {
    size_t size = expression_to_rax(contents[1], asm_m);
    asm_m.add_instr_main("push rax");

    size_t size2 = expression_to_rax(contents[0], asm_m);
    asm_m.add_instr_main("pop rbx");
    asm_m.add_instr_main("add rax, rbx");

    if (size2 > size) return size2;
    return size;
  }
  else if (astree.value() == "-" && astree.size() == 2) {
    size_t size = expression_to_rax(contents[1], asm_m);
    asm_m.add_instr_main("push rax");

    size_t size2 = expression_to_rax(contents[0], asm_m);
    asm_m.add_instr_main("pop rbx");
    asm_m.add_instr_main("sub rax, rbx");

    if (size2 > size) return size2;
    return size;
  }
  else if (astree.value() == "*" && astree.size() == 2) {
    size_t size = expression_to_rax(contents[1], asm_m);
    asm_m.add_instr_main("push rax");

    size_t size2 = expression_to_rax(contents[0], asm_m);
    asm_m.add_instr_main("pop rbx");
    asm_m.add_instr_main("imul rax, rbx");

    if (size2 > size) return size2;
    return size;
  }
  else if (astree.value() == "/" && astree.size() == 2) {
    size_t size = expression_to_rax(contents[1], asm_m);
    asm_m.add_instr_main("push rax");

    size_t size2 = expression_to_rax(contents[0], asm_m);
    asm_m.add_instr_main("pop rbx");
    asm_m.add_instr_main("idiv rbx");

    if (size2 > size) return size2;
    return size;
  }

  std::cout << "PANICKING: expression_to_rax didn't return. There is some serious shit going on, not with your code but with hydro itself." << std::endl;
  std::cout << astree.value()<< std::endl;
  exit(1);
}
