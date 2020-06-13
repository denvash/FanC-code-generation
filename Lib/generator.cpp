#include "generator.hpp"
#include "llvm_code.hpp"

Generator *Generator::instance = Generator::getInstance();

void Generator::generate()
{
  debugGenerator("generate");
  CodeBuffer::instance().emitGlobal(printf_llvm);
  CodeBuffer::instance().emitGlobal(print_llvm);
  CodeBuffer::instance().emit(main_llvm);

  CodeBuffer::instance().printGlobalBuffer();
  CodeBuffer::instance().printCodeBuffer();
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