#include "include/ASM_manager.h"

#include <algorithm>
#include <iostream>

void ASM_manager::add_instr_backend(std::string key, std::string value, std::unordered_map<std::string, std::string> *map, std::stringstream *stream) {
  if(map -> find(key) != map -> end()) {

  } else {
    *stream << "    " + value + "\n";
    (*map)[key] = value;
  }
}

Mem_location ASM_manager::define_local_var(std::string identifier, size_t size_r) {
  size_t offset_r = scope_size;
  scope_size += size_r;
  auto loc = Mem_location{.size=size_r, .offset=offset_r};

  memmap.insert (std::make_pair(identifier, loc));

  return loc;
}

Mem_location ASM_manager::get_local_var(std::string identifier) {
      if(memmap.find(identifier) != memmap.end()) {
        return memmap.at(identifier);
        std::cout << "found a " << identifier;
      }
      else {
        std::cout << "found no " << identifier;
        return {.size = 0, .offset = 0};
      }
  }

std::string ASM_manager::str_main() {
      std::stringstream pre;
      std::stringstream post;
      post << "_hy_main_ret:\n";
      if (scope_size != 0) {
        char fill = (16 - scope_size % 16);
        if (fill == 16) {
          fill = 0;
        }
        scope_size += fill;

        pre << "    mov rax, 0xFFFFFFFF\n";
        pre << "    push rax\n";
        pre << "    push rbp\n    mov rbp, rsp\n";
        pre << "    sub rsp, " << scope_size << "\n";
        post << "    add rsp, " << scope_size << "\n";
      }
      post << "    pop rbp\n";
      post << "    pop rax\n";
      if (return_from_scope) post << "    ret";
      return pre.str() + stream_main.str() + post.str();
}
