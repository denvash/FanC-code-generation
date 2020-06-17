#include "generator.hpp"
#include "llvm_code.hpp"

Generator *Generator::instance = Generator::getInstance();
#define _B (CodeBuffer::instance())
#define PRINT_FUNC "print"
#define PRINTI_FUNC "printi"
#define PLACE_VOID "void"

void Generator::generate()
{
  // debugGenerator("Generating LLVM Code");
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
  // debugGenerator("Func Init", func_id.c_str());

  // Function must exists in this point.
  auto entry = table.get_last_function_in_scope();
  auto type_info = entry->type_info;

  auto total_args = table.get_total_args(func_id);

  auto type = type_info.type;
  auto arg_types = type_info.arg_types;
  auto offset = entry->offset;

  auto total_args_str = to_string(total_args);
  // debugGenerator("Func info: Type,Size", type_to_string_map[type] + "," + total_args_str);

  _B.emit(define_func_llvm(type == TYPE_VOID, total_args, func_id));
  _B.emit(func_entry_llvm);

  if (total_args > 0)
  {
    _B.emit(define_args_llvm(total_args_str));
  }
  for (auto i = 0; i < total_args; i++)
  {
    auto var = _gen_var_llvm();
    _B.emit(declare_var_llvm(var, total_args_str, (i + 1) * (-1)));
    _B.emit(store_arg_llvm(to_string(i), var));
  }
}

void Generator::func_end(atom_t &atom_id, atom_t &atom_statement)
{
  // debugGenerator("Func End: ", *atom_statement.STRING);

  auto type_info = table.get_last_function_in_scope();
  auto branch_to_bp = _B.emit(branch_to_bp_llvm);
  auto label = _B.genLabel();

  _B.bpatch(_B.makelist({branch_to_bp, FIRST}), label);
  _B.bpatch(atom_statement.next_list, label);
  _B.emit(atom_id.TYPE == TYPE_VOID ? ret_void_llvm : ret_success_llvm);
  _B.emit(scope_end_llvm);
}

void Generator::func_call(atom_t &$$, atom_t &atom_id, atom_t &atom_exp_list)
{
  auto func_name = *(atom_id.STRING);
  auto exp_list = dynamic_cast<ExpList *>(atom_exp_list.NODE);

  if (func_name == PRINT_FUNC)
  {
    // debugGenerator("Print func call");
    auto str_atom = exp_list->list.front();
    auto str_value = *(str_atom.STRING);
    // debugGenerator(str_value.c_str());

    /* len without quotes */
    auto str_len = to_string(str_value.length() - 1);
    auto var_id = str_atom.VAR_ID;

    _B.emit(call_print_llvm(str_len, var_id));

    $$.place = PLACE_VOID;
    return;
  }

  auto function_type = table.get_function_type(func_name);
  string args_llvm = "";

  for (auto &&exp_atom : exp_list->list)
  {
    auto expression_value = to_string(exp_atom.INT);
    args_llvm = args_llvm + "i32 " + expression_value + ",";
  }
  /* Cut the last "," */
  args_llvm = args_llvm.substr(0, args_llvm.length() - 1);

  auto call_exp_llvm = call_function_llvm(function_type == TYPE_VOID, func_name, args_llvm);

  if (function_type == TYPE_VOID)
  {
    _B.emit(call_exp_llvm);
    $$.place = PLACE_VOID;
  }
  /* There is a return type */
  else
  {
    auto var = _gen_string_var_id_llvm();
    _B.emit(assign_to_var_llvm(var, call_exp_llvm));
    $$.place = PLACE_VOID;
  }
}

void Generator::gen_string(atom_t &atom)
{
  // debugGenerator("Generating String");
  auto str_full_value = *(atom.STRING);
  // cout << "[GenString]: " << str_full_value << endl;

  /* full value comes as "STRING" and we want only the value: STRING */
  auto str_value = str_full_value.substr(1, str_full_value.length() - 2);
  atom.VAR_ID = _gen_string_var_id_llvm();
  auto str_len = to_string(str_value.length() + 1);
  _B.emitGlobal(store_string_llvm(atom.VAR_ID, str_len, str_value));
}

void Generator::gen_bp_label(atom_t &$$)
{
  // debugGenerator("Generating BP Label");
  auto buffer_index = _B.emit(branch_to_bp_llvm);
  auto label = _B.genLabel();
  _B.bpatch(_B.makelist({buffer_index, FIRST}), label);
  $$.quad = label;
}

void Generator::gen_binop(atom_t &$$, atom_t &atom_left, atom_t &atom_op, atom_t &atom_right)
{
  auto op = *(atom_op.STRING);
  // debugGenerator("BINOP", op);
  auto var = _gen_var_llvm();

  if (op == "/")
  {
    auto zero_var_llvm = _gen_var_llvm();
    auto left_value = to_string(atom_left.INT);
    auto right_value = to_string(atom_right.INT);

    _B.emit(zero_div_check_llvm(zero_var_llvm, right_value));
    auto zero_bp = _B.emit(branch_conditional_to_bp_llvm(zero_var_llvm));

    auto error_label = _B.genLabel();
    _B.emit(call_print_zero_div_llvm);

    auto unused_bp = _B.emit(branch_to_bp_llvm);

    auto success_label = _B.genLabel();
    _B.emit(op_div_llvm(var, "i32", left_value, right_value));

    _B.bpatch(_B.makelist({zero_bp, SECOND}), success_label);
    _B.bpatch(_B.makelist({zero_bp, FIRST}), error_label);
    _B.bpatch(_B.makelist({unused_bp, FIRST}), success_label);
  }
}

void Generator::gen_assign(atom_t &$$, atom_t &atom_id, atom_t &atom_assign, atom_t &atom_exp)
{
  debugGenerator("Generating Assign");
  if (atom_assign.TYPE != TYPE_BOOL)
  {
    auto var = _gen_var_llvm();
    auto atom_func = table.get_last_function_in_scope();
    auto offset = atom_func->offset;
    auto func_id = atom_func->name;
    auto args_size = table.get_total_args(func_id);

    _B.emit(declare_var_llvm(var, to_string(offset), args_size));

    /* TODO: fix atom exp place */
    _B.emit(store_arg_llvm(atom_exp.place, var));
    $$.next_list = atom_exp.next_list;
  }
}