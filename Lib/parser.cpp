#include "parser.hpp"

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

SemanticTable table;
int while_scope_count = 0;

/* Order matters */
int type_size_map[HELP_TYPE_NUM] = {
    0, // TYPE_NONE
    0, // TYPE_VOID
    1, // TYPE_BOOL
    1, // TYPE_INT
    1, // TYPE_BYTE
    1, // TYPE_STRING
    1  // TYPE_B
};

/* Order matters */
string type_to_string_map[HELP_TYPE_NUM] = {
    "",
    "VOID",
    "BOOL",
    "INT",
    "BYTE",
    "STRING",
    "B",
};

/* Order matters */
bool converstion_map[HELP_TYPE_NUM][HELP_TYPE_NUM] = {
    /* TYPE_UNDEFINED to  */ {
        /* TYPE_UNDEFINED */ true,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_VOID to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ true,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_BOOL to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ true,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_INT to        */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ true,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
    },
    /* TYPE_BYTE to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ true,
        /* TYPE_BYTE      */ true,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_STRING to     */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ true,
        /* TYPE_B         */ false,
    },
    /* TYPE_B to */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ true,
        /* TYPE_B         */ true,
    }};

inline void debugParser(const char *text)
{
  debug("parser", text);
}

void init_program()
{
  table.init_table();
  table.open_scope();
  /* Insert print and printi to global scope */
  vector<TypeEnum> func_args;

  func_args.push_back(TYPE_STRING);
  var_info_t print_func = {true, 0, TYPE_VOID, func_args};
  func_args.pop_back();

  table.insert("print", &print_func, true);
  func_args.push_back(TYPE_INT);

  var_info_t printi_func = {true, 0, TYPE_VOID, func_args};
  table.insert("printi", &printi_func, true);
}

void close_scope()
{
  // debugParser("[parser:close-scope] closing scope");
  output::endScope();
  auto entries = table.get_current_inner_scope();
  for (auto i = 0; i < entries.size(); i++)
  {
    auto type_str = type_to_string_map[entries[i]->type_info.type];
    auto id = entries[i]->name;
    auto offset = entries[i]->offset;

    // cout << "[parser:entry-id] " << id << endl;

    if (entries[i]->type_info.is_func)
    {
      auto function_entry = entries[i];
      vector<string> func_args;
      for (auto i = 0; i < function_entry->type_info.arg_types.size(); i++)
      {
        auto arg_str = type_to_string_map[function_entry->type_info.arg_types[i]];
        arg_str = (arg_str == "VOID") ? "" : arg_str;
        func_args.push_back(arg_str);
      }
      auto function_return_type_str = type_to_string_map[function_entry->type_info.type];
      type_str = output::makeFunctionType(function_return_type_str, func_args);
    }
    output::printID(id, offset, type_str);
  }
  table.close_scope();
}

void close_program()
{
  // debugParser("close program");
  if (!table.is_func_exists(MAIN_FUNC))
  {
    // debugParser("No func id_is_exists");
    err(output::errorMainMissing);
  }
  if (table.get_function_args(MAIN_FUNC).front() != TYPE_VOID)
  {
    // debugParser("Main with args");
    err(output::errorMainMissing);
  }
  if (table.get_function_type(MAIN_FUNC) != TYPE_VOID)
  {
    // debugParser("wrong type");
    err(output::errorMainMissing);
  }
  close_scope();
}

void return_value_check(TypeEnum return_type)
{
  // debugParser("return value check");
  auto is_valid_converstion = converstion_map[return_type][table.get_current_function_type()];
  if (!is_valid_converstion)
  {
    err(output::errorMismatch, yylineno);
  }
}

void func_init(atom_t y_identifier, atom_t y_arguments)
{
  // debugParser("declaring function");
  // cout << "[parser:function] " << *y_identifier.STRING << endl;
  if (table.id_is_exists(*y_identifier.STRING))
  {
    err(output::errorDef, yylineno, *y_identifier.STRING);
  }
  var_info_t func;
  func.is_func = true;
  func.type = y_identifier.TYPE;

  /* if no arguments to function, args is null */
  if (y_arguments.NODE == NULL)
  {
    // debugParser("In declaring function, no arguments");
    func.arg_types.push_back(TYPE_VOID);
  }
  else
  {
    auto args = dynamic_cast<FormalsList *>(y_arguments.NODE);
    for (auto i = 0; i < args->list.size(); i++)
    {
      func.arg_types.push_back(args->list[i].TYPE);
    }
  }
  func.size = 0;
  table.insert(*y_identifier.STRING, &func, true);
  table.func_info = func;
}

void variable_init(atom_t y_identifier, bool is_local)
{
  // debugParser("Declaring var");
  // cout << "[parser:var]: " << type_to_string_map[y_identifier.TYPE] << endl;
  // cout << "[parser:STRING]: " << *y_identifier.STRING << endl;
  if (table.id_is_exists(*y_identifier.STRING))
  {
    err(output::errorDef, yylineno, *y_identifier.STRING);
  }
  var_info_t var_info;
  var_info.is_func = false;
  var_info.type = y_identifier.TYPE;
  var_info.size = type_size_map[y_identifier.TYPE];
  table.insert(*y_identifier.STRING, &var_info, is_local);
}

void declare_formals(atom_t yy_formals)
{
  if (yy_formals.NODE == NULL)
  {
    return;
  }
  auto formals_list = dynamic_cast<FormalsList *>(yy_formals.NODE);
  for (auto i = 0; i < formals_list->list.size(); i++)
  {
    variable_init(formals_list->list[i], false);
  }
  table.offsets.top() = 0;
}

Exp::Exp(atom_t a, string op, atom_t b)
{
  auto mismatch = false;
  if (op == AND)
  {
    if (a.TYPE != TYPE_BOOL || b.TYPE != TYPE_BOOL)
      mismatch = true;
    value = a.INT && b.INT;
    type = TYPE_BOOL;
  }
  else if (op == OR)
  {
    if (a.TYPE != TYPE_BOOL || b.TYPE != TYPE_BOOL)
      mismatch = true;
    value = a.INT || b.INT;
    type = TYPE_BOOL;
  }
  else if (op == PLUS)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.INT + b.INT;
  }
  else if (op == MINUS)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.INT - b.INT;
  }
  else if (op == MUL)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.INT * b.INT;
  }
  else if (op == DIV)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
  }
  else if (op == LS)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT < b.INT;
  }
  else if (op == LS_EQ)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT <= b.INT;
  }
  else if (op == EQUAL)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT == b.INT;
  }
  else if (op == GR_EQ)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT >= b.INT;
  }
  else if (op == GR)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT > b.INT;
  }
  else if (op == NOT_EQ)
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT != b.INT;
  }
  if (mismatch)
    err(output::errorMismatch, yylineno);
}

Call::Call(atom_t identifier, atom_t yy_exp_list)
{
  if (!table.is_func_exists(*identifier.STRING))
  {
    err(output::errorUndefFunc, yylineno, *identifier.STRING);
  }

  auto exp_list = dynamic_cast<ExpList *>(yy_exp_list.NODE);
  auto exp_type_list = exp_list->list;
  auto args_type = table.get_function_args(*identifier.STRING);

  if (args_type.size() != exp_type_list.size())
  {
    // debugParser("Function Call: Arguments list size mismatch");
    // cout << "[parser:Call:expected list size] " << exp_type_list.size() << endl;
    // cout << "[parser:Call:args list size] " << args_type.size() << endl;
    vector<string> args;
    for (auto i = 0; i < args_type.size(); i++)
    {
      args.push_back(type_to_string_map[args_type[i]]);
      auto typeVoid = type_to_string_map[TYPE_VOID];
      auto typeUndefined = type_to_string_map[TYPE_UNDEFINED];
      args.front() = args.front() == typeVoid ? typeUndefined : args.front();
    }
    err(output::errorPrototypeMismatch, yylineno, *identifier.STRING, args);
  }

  for (auto i = 0; i < args_type.size(); i++)
  {
    auto is_valid = converstion_map[exp_type_list[i].TYPE][args_type[i]];
    if (!is_valid)
    {
      // debugParser("Function Call: Type mismatch");
      vector<string> args;
      for (auto j = 0; j < args_type.size(); j++)
      {
        args.push_back(type_to_string_map[args_type[j]]);
      }
      err(output::errorPrototypeMismatch, yylineno, *identifier.STRING, args);
    }
  }

  this->type = table.get_function_type(*identifier.STRING);
}

Call::Call(atom_t identifier)
{
  if (!table.is_func_exists(*identifier.STRING))
  {
    err(output::errorUndefFunc, yylineno, *identifier.STRING);
  }
  auto args_type = table.get_function_args(*identifier.STRING);
  if (args_type.size() != 1 || args_type.front() != TYPE_VOID)
  {
    vector<string> args;
    for (auto j = 0; j < args_type.size(); j++)
    {
      args.push_back(type_to_string_map[args_type[j]]);
      args.front() = args.front() == "VOID" ? "" : args.front();
    }

    err(output::errorPrototypeMismatch, yylineno, *identifier.STRING, args);
  }
  this->type = table.get_function_type(*identifier.STRING);
}

void assign_value(atom_t y_identifier, atom_t y_expression)
{
  if (!table.id_is_exists(*y_identifier.STRING))
  {
    err(output::errorUndef, yylineno, *y_identifier.STRING);
  }
  auto entry = table.get_entry(*y_identifier.STRING);
  if (entry.type_info.is_func)
  {
    err(output::errorUndef, yylineno, *y_identifier.STRING);
  }
  auto is_valid_converstion = converstion_map[y_expression.TYPE][entry.type_info.type];
  if (!is_valid_converstion)
  {
    err(output::errorMismatch, yylineno);
  }
}
