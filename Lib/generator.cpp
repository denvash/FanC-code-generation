#include "generator.hpp"
#include "llvm_code.hpp"

Generator *Generator::instance = Generator::getInstance();
#define _B (CodeBuffer::instance())
#define PRINT_FUNC "print"
#define PRINTI_FUNC "printi"

void Generator::generate()
{
  debugGenerator("Generating LLVM Code");
  _B.emitGlobal(printf_llvm);
  _B.emitGlobal(print_llvm);
  _B.emitGlobal(printi_llvm);
  _B.emitGlobal(zero_div_llvm);
  _B.emitGlobal(exit_llvm);

  _B.printGlobalBuffer();
  _B.printCodeBuffer();
}

void Generator::func_init(atom_t &$$)
{
  auto func_id = *$$.STRING;
  debugGenerator("Func Init", func_id.c_str());

  // Function must exists in this point.
  auto entry = table.get_last_function_in_scope();
  auto type_info = entry->type_info;
  auto size = type_info.size;
  auto type = type_info.type;
  auto arg_types = type_info.arg_types;
  auto offset = entry->offset;
  auto size_str = to_string(size);

  debugGenerator("Func info: Type,Size", type_to_string_map[type] + "," + size_str);

  _B.emit(define_func_llvm(type == TYPE_VOID, size, func_id));

  if (size > 0)
  {
    _B.emit(define_args_llvm(size_str));
  }
  for (auto i = 0; i < size; i++)
  {
    auto var = _generate_var();
    _B.emit(declare_var_llvm(var, size_str, offset));
    _B.emit(store_arg_llvm(to_string(i), size_str));
  }
}

void Generator::func_end(atom_t &atom_id, atom_t &atom_statement)
{
  debugGenerator("Func End: ", *atom_statement.STRING);

  auto type_info = table.get_last_function_in_scope();
  auto branch_to_bp = _B.emit(branch_to_bp_llvm);
  auto label = _B.genLabel();

  _B.bpatch(_B.makelist({branch_to_bp, FIRST}), label);
  _B.bpatch(type_info->next_list, label);
  _B.emit(atom_id.TYPE == TYPE_VOID ? ret_void_llvm : ret_success_llvm);
  _B.emit(scope_end_llvm);
}

void Generator::func_call(atom_t &$$, atom_t &id_atom, atom_t &exp_list_atom)
{
  debugGenerator("Func Call");
  auto func_name = *(id_atom.STRING);
  auto exp_list = dynamic_cast<ExpList *>(exp_list_atom.NODE);

  if (func_name == PRINT_FUNC)
  {
    debugGenerator("Print func call");
    auto str_atom = exp_list->list.front();
    auto str_value = *(str_atom.STRING);
    debugGenerator(str_value.c_str());

    /* len without quotes */
    auto str_len = to_string(str_value.length() - 1);
    auto var_id = str_atom.VAR_ID;

    _B.emit(call_print_llvm(str_len, var_id));

    $$.VAR_ID = "void";
    return;
  }
  else if (func_name == PRINTI_FUNC)
  {
    debugGenerator("Print-i func call");
  }
}

void Generator::gen_string(atom_t &atom)
{
  debugGenerator("Generating String");
  auto str_full_value = *(atom.STRING);
  cout << "[GenString]: " << str_full_value << endl;

  /* full value comes as "STRING" and we want only the value: STRING */
  auto str_value = str_full_value.substr(1, str_full_value.length() - 2);
  atom.VAR_ID = _gen_string_var_id();
  auto str_len = to_string(str_value.length() + 1);
  _B.emitGlobal(store_string_llvm(atom.VAR_ID, str_len, str_value));
}