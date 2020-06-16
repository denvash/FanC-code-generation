#ifndef _GENERATOR_HPP
#define _GENERATOR_HPP

#include "parser.hpp"
#include "register_handler.hpp"
#include "bp.hpp"

class Generator
{
private:
  static Generator *instance;
  static CodeBuffer *code_buffer;
  map<int, RegisterHandler> register_map;
  map<int, vector<int>> true_lists;
  map<int, vector<int>> continue_lists;
  map<int, vector<int>> false_lists;
  map<int, vector<int>> next_lists;
  map<int, string> quad_list;

  size_t total_vars;

  Generator() = default;

  size_t _gen_id()
  {
    return total_vars++;
  };

  string _generate_var()
  {
    return "%var" + std::to_string(_gen_id());
  };

  string _gen_string_var()
  {
    return "@.str_" + std::to_string(_gen_id());
  };

public:
  static Generator *getInstance()
  {
    if (!instance)
    {
      instance = new Generator;
    }
    return instance;
  }

  void generate();
  void func_init(atom_t &atom);
  void func_end();
  void func_call();
  void gen_string(atom_t &atom);
};

inline void debugGenerator(const char *text)
{
  debug("Generator", text);
}

#endif