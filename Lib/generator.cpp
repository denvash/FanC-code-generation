#include "generator.hpp"
#include "llvm_code.hpp"

Generator *Generator::instance = Generator::getInstance();
#define _B (CodeBuffer::instance())

void Generator::generate()
{
  debugGenerator("generate");
  _B.emitGlobal(printf_llvm);
  _B.emitGlobal(print_llvm);
  _B.emitGlobal(printi_llvm);
  _B.emitGlobal(zero_div_llvm);
  _B.emitGlobal(exit_llvm);

  _B.printGlobalBuffer();
  _B.printCodeBuffer();
}

void Generator::func_init(atom_t &atom)
{
  string func_id = *atom.STRING;
  debugGenerator("Func Init");
}

void Generator::func_end()
{
  debugGenerator("Func End");
}

void Generator::func_call()
{
  debugGenerator("Func Call");
}

void Generator::gen_string(atom_t &atom)
{
  auto str_value = *(atom.STRING);
  debugGenerator("Generating String");
  cout << "[GenString]: " << str_value << endl;

  auto str_var = _gen_string_var();
  auto str_len = std::to_string(str_value.length() + 1);
  auto str_var_llvm = str_var + " = constant [" + str_len + " x i8] c\"" + str_value + "\\00\"";
  _B.emitGlobal(str_var_llvm);
}