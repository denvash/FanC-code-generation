#include "generator.hpp"
#include "llvm_code.hpp"

Generator *Generator::instance = Generator::getInstance();
#define _B (CodeBuffer::instance())

void Generator::generate()
{
  debugGenerator("generate");
  _B.emitGlobal(printf_llvm);
  _B.emitGlobal(print_llvm);
  _B.emit(main_llvm);

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

void Generator::func_call(atom_t &target, atom_t &y_function_id)
{
  debugGenerator("Func Call");
}