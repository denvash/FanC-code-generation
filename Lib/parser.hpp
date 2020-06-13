#ifndef PARSER_HPP
#define PARSER_HPP
#include "output.hpp"
#include "table.hpp"
#include "err.hpp"
#include "atoms.hpp"
using namespace std;
#define YYSTYPE atom_t
extern int while_scope_count;
void init_program();
void close_program();
void return_value_check(TypeEnum return_type);
void close_scope();
void func_init(atom_t y_identifier, atom_t y_arguments);
void declare_formals(atom_t yy_formals);
void assign_value(atom_t y_identifier, atom_t y_expression);
void variable_init(atom_t y_identifier, bool is_local);
#endif /* PARSER_HPP */