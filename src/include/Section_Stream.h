#ifndef SECTION_STREAM_H_
#define SECTION_STREAM_H_

#include <sstream>
#include <vector>
#include <string>

class Section_Stream {
  public:
    std::string str() {return stream.str();}
    void add_instr(std::string key, std::string value);
  private:
    std::vector<std::string> keys;
    std::stringstream stream;
};

#endif // SECTION_STREAM_H_
