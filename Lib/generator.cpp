#include "generator.hpp"
#include "llvm_code.hpp"

Generator *Generator::instance = Generator::getInstance();
#define _B (CodeBuffer::instance())
#define PRINT_FUNC "print"
#define PRINTI_FUNC "printi"
#define PLACE_VOID "void"

#define MAIN_FUNC "main"
#define AND "and"
#define OR "or"
#define PLUS "+"
#define MINUS "-"
#define MUL "*"
#define DIV "/"
#define GR ">"
#define GR_EQ ">="
#define LS "<"
#define LS_EQ "<="
#define EQUAL "=="
#define NOT_EQ "!="

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
    auto target = _gen_var_llvm();
    _B.emit(declare_var_llvm(target, total_args_str, (i + 1) * (-1)));
    _B.emit(store_arg_through_id_llvm(to_string(i), target));
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
    auto source = str_atom.place == "" ? str_atom.VAR_ID : str_atom.place;

    _B.emit(call_print_llvm(str_len, source));

    $$.place = PLACE_VOID;
    return;
  }

  auto function_type = table.get_function_type(func_name);
  string args_llvm = "";

  for (auto &&exp_atom : exp_list->list)
  {
    auto exp_value = to_string(exp_atom.INT);
    auto exp_place = exp_atom.place;
    auto exp_source = exp_place == "" ? exp_value : exp_place;
    args_llvm = args_llvm + "i32 " + exp_source + ",";
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
    auto target = _gen_var_llvm();
    _B.emit(assign_to_var_llvm(target, call_exp_llvm));
    $$.place = target;
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
  auto target = _gen_var_llvm();

  auto right_value = to_string(atom_right.INT);
  auto left_value = to_string(atom_left.INT);

  auto right_type = atom_right.TYPE;
  auto left_type = atom_left.TYPE;

  auto right_place = atom_right.place;
  auto left_place = atom_left.place;

  /* The values to use */
  auto right = right_place == "" ? right_value : right_place;
  auto left = left_place == "" ? left_value : left_place;

  auto can_overflow = false;
  /* Byte overflow */
  if (op != DIV && left_type == TYPE_BYTE && right_type == TYPE_BYTE)
  {

    auto left_var = _gen_var_llvm();
    _B.emit(assign_byte_llvm(left_var, left));

    auto right_var = _gen_var_llvm();
    _B.emit(assign_byte_llvm(right_var, right));

    /* Bubble the updates */
    left = left_var;
    right = right_var;

    can_overflow = true;
  }

  auto op_type_llvm = can_overflow ? "i8" : "i32";

  if (op == DIV)
  {
    auto zero_var_llvm = _gen_var_llvm();
    auto zero_source = right_place == "" ? right_value : right_place;

    _B.emit(zero_div_check_llvm(zero_var_llvm, zero_source));
    auto zero_bp = _B.emit(branch_conditional_to_bp_llvm(zero_var_llvm));

    auto error_label = _B.genLabel();
    _B.emit(call_print_zero_div_llvm);

    auto unused_bp = _B.emit(branch_to_bp_llvm);

    auto success_label = _B.genLabel();
    _B.emit(assign_op_llvm(target, "sdiv", op_type_llvm, left, right));

    _B.bpatch(_B.makelist({zero_bp, SECOND}), success_label);
    _B.bpatch(_B.makelist({zero_bp, FIRST}), error_label);
    _B.bpatch(_B.makelist({unused_bp, FIRST}), success_label);
  }
  else if (op == MUL)
  {
    _B.emit(assign_op_llvm(target, "mul", op_type_llvm, left, right));
  }
  else if (op == PLUS)
  {
    _B.emit(assign_op_llvm(target, "add", op_type_llvm, left, right));
  }
  else if (op == MINUS)
  {
    _B.emit(assign_op_llvm(target, "sub", op_type_llvm, left, right));
  }

  /* Handle overflow / sign */
  if (can_overflow)
  {
    auto next_var = _gen_var_llvm();
    _B.emit(assign_byte_overflow_llvm(next_var, target));
    target = next_var;
  }
  $$.place = target;
}

void Generator::gen_assign(atom_t &$$, atom_t &atom_id, atom_t &atom_assign, atom_t &atom_exp)
{
  if (atom_assign.TYPE == TYPE_BOOL)
  {
    debugGenerator("Boolean Assign");
    return;
  }

  // debugGenerator("Non Boolean Assign");
  auto target = _gen_var_llvm();

  auto atom_func = table.get_last_function_in_scope();
  auto func_name = atom_func->name;

  auto args_size = table.get_total_args(func_name);
  auto args_size_str = to_string(args_size);

  auto id_entry = table.get_entry(*atom_id.STRING);
  auto id_offset = id_entry.offset;
  // debugGenerator("offset:", to_string(id_offset));

  _B.emit(declare_var_llvm(target, args_size_str, id_offset));

  _B.emit(store_arg_through_place_llvm(atom_exp.place, target));
  $$.next_list = atom_exp.next_list;
}
void Generator::gen_id(atom_t &$$, atom_t &atom_id)
{
  // debugGenerator("Generating Id");

  auto source = _gen_var_llvm();
  auto target = _gen_var_llvm();

  auto entry = table.get_last_function_in_scope();

  auto func_info = entry->type_info;
  auto func_name = entry->name;

  auto id_entry = table.get_entry(*atom_id.STRING);
  auto args_size = table.get_total_args(func_name);
  auto args_size_str = to_string(args_size);

  auto id_offset = id_entry.offset;

  // debugGenerator("atom id:", *atom_id.STRING);
  // debugGenerator("func name:", func_name);
  // debugGenerator("args size", args_size_str);
  // debugGenerator("offset", to_string(id_offset));

  _B.emit(declare_var_llvm(source, args_size_str, id_offset));
  _B.emit(load_to_register_llvm(target, source));
  $$.place = target;

  if (atom_id.TYPE == TYPE_BOOL)
  {
    auto target_boolean = _gen_var_llvm();
    _B.emit(compare_boolean_llvm(target_boolean, source));
    auto branch_index = _B.emit(branch_conditional_to_bp_llvm(target_boolean));
    auto unconditional_list = _B.makelist({branch_index, FIRST});
    auto conditional_list = _B.makelist({branch_index, SECOND});

    $$.true_list = unconditional_list;
    $$.false_list = conditional_list;
  }
}

void Generator::gen_return(atom_t &$$)
{
  _B.emit(ret_void_llvm);
}

void Generator::gen_return_exp(atom_t &$$, atom_t &atom_exp)
{
  auto place = atom_exp.place;
  auto value = to_string(atom_exp.INT);
  auto source = place == "" ? value : place;
  _B.emit(ret_exp_llvm(source));
  $$.next_list = atom_exp.next_list;
}