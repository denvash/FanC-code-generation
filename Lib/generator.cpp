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
  string llvm;
  string func_id = *atom.STRING;
  debugGenerator("Generate Func Init");
  cout << func_id << endl;
  CodeBuffer::instance().emit("\n\n@" + func_id + "\n");
}