#ifndef SECTION_STREAM_H_
#define SECTION_STREAM_H_

#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

struct Mem_location {
  size_t size;
  size_t offset;
};

class ASM_manager {
  public:
    bool return_from_scope = false;
    inline void add_instr_text(std::string key, std::string value) {add_instr_backend(key, value, &map_text, &stream_text);}
    inline void add_instr_data(std::string key, std::string value) {add_instr_backend(key, value, &map_data, &stream_data);}
    inline void add_instr_main(std::string value) {stream_main << "    " << value << "\n";}

    std::string str_main();
    inline std::string str_data() {return stream_data.str();}
    inline std::string str_text() {return stream_text.str();}

    Mem_location define_local_var (std::string identifier, size_t size_r);
    Mem_location get_local_var (std::string identifier);
  private:
    void add_instr_backend(std::string key, std::string value, std::unordered_map<std::string, std::string> *map, std::stringstream *stream);

    std::unordered_map<std::string, std::string> map_data;
    std::unordered_map<std::string, std::string> map_text;
    std::stringstream stream_text;
    std::stringstream stream_data;

    std::stringstream stream_main;

    std::unordered_map<std::string, Mem_location> memmap;
    size_t scope_size = 0;
};

#endif // SECTION_STREAM_H_
