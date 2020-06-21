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
  size_t total_vars;

  Generator() = default;

  size_t _gen_id()
  {
    return total_vars++;
  };

  string _gen_var_llvm()
  {
    return "%var" + std::to_string(_gen_id());
  };

  string _gen_string_var_id_llvm()
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
  void func_init(atom_t &);
  void func_end(atom_t &, atom_t &);
  void func_call(atom_t &, atom_t &, atom_t &);
  void func_call(atom_t &, atom_t &); /* Func call without args */
  void gen_string(atom_t &);
  void gen_bp_label(atom_t &);
  void gen_binop(atom_t &, atom_t &, atom_t &, atom_t &);
  void gen_assign(atom_t &, atom_t &, atom_t &, atom_t &);
  void gen_id(atom_t &, atom_t &);
  void gen_return(atom_t &);
  void gen_return_exp(atom_t &, atom_t &);
  void gen_relop(atom_t &, atom_t &, atom_t &, atom_t &);
  void gen_logicalop(atom_t &, atom_t &, atom_t &, atom_t &);
  void gen_bp_label_makelist(atom_t &);
  void gen_eval_boolean(atom_t &, atom_t &);
  void gen_bp_boolean_exp(atom_t &, atom_t &, atom_t &);
  void gen_bp_boolean_in_statement(atom_t &, atom_t &, atom_t &);
  void makelist_boolean(atom_t &, bool);
};

inline void debugGenerator(string text)
{
  debug("Generator", text.c_str());
}

inline void debugGenerator(string pre, string text)
{
  auto str = "Generetor: " + pre;
  debug(str.c_str(), text.c_str());
}

#endif