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

  Generator() = default;

  void emit_global_string(const string &name, const string &str);
  void emit_global_string(int id, const string &str);
  void sp_pop(string popped, bool inc);
  void sp_push(string pushed, bool dec);

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
};

inline void debugGenerator(const char *text)
{
  debug("parser", text);
}

#endif