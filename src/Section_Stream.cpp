#include "include/Section_Stream.h"

#include <algorithm>

void Section_Stream::add_instr(std::string key, std::string value) {
  if(std::find(keys.begin(), keys.end(), key) != keys.end()) {

  } else {
    stream << "    " + value;
  }
}
