#include "generator.hpp"
#include "bp.hpp"

Generator *Generator::instance = Generator::getInstance();

static const string print_llvm("print");

void Generator::generate()
{
  debugGenerator("generate");
  CodeBuffer::instance().emit(print_llvm);
  CodeBuffer::instance().printCodeBuffer();
}