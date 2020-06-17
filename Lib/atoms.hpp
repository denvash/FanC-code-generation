#include "types.hpp"
#include "table.hpp"
#include "err.hpp"
#include "output.hpp"
#include "bp.hpp"
#include <string>
#include <stdlib.h>

extern SemanticTable table;
extern int yylineno;
using namespace std;

class Node
{
public:
  virtual int get_value()
  {
    return 0;
  }
  virtual TypeEnum get_type()
  {
    return TYPE_UNDEFINED;
  }
};

struct atom_t
{
  string VAR_ID;
  int INT;
  TypeEnum TYPE;
  string *STRING;
  Node *NODE;
  vector<pair<int, BranchLabelIndex>> next_list;
  vector<pair<int, BranchLabelIndex>> true_list;
  vector<pair<int, BranchLabelIndex>> false_list;
  vector<pair<int, BranchLabelIndex>> break_list;
  vector<pair<int, BranchLabelIndex>> continue_list;
};

class Exp : public Node
{
public:
  int value;
  string id;
  TypeEnum type;

  Exp(string identifier)
  {
    if (!table.is_var_exists(identifier))
      err(output::errorUndef, yylineno, identifier);
    auto entry = table.get_entry(identifier);
    id = identifier;
    type = entry.type_info.type;
  }
  Exp(atom_t a, string op, atom_t b);
  int get_value()
  {
    return this->value;
  }
  TypeEnum get_type()
  {
    return this->type;
  }
};

class FormalsList : public Node
{
public:
  vector<atom_t> list;
  FormalsList(atom_t yy_formal)
  {
    this->list.push_back(yy_formal);
  }
  FormalsList(atom_t yy_formals_list, atom_t yy_formal)
  {
    auto formals_list = dynamic_cast<FormalsList *>(yy_formals_list.NODE);
    this->list.insert(this->list.begin(), formals_list->list.begin(), formals_list->list.end());
    this->list.push_back(yy_formal);
  }
};

class IfExp : public Node
{
public:
  IfExp(atom_t exp)
  {
    if (exp.TYPE != TYPE_BOOL)
    {
      err(output::errorMismatch, yylineno);
    }
  }
};

class Call : public Node
{
public:
  TypeEnum type;
  int value;
  Call(atom_t identifier);
  Call(atom_t identifier, atom_t yy_exp_list);
  TypeEnum get_type()
  {
    return this->type;
  }
};

class ExpList : public Node
{
public:
  vector<atom_t> list;
  ExpList(atom_t exp)
  {
    list.push_back(exp);
  }
  ExpList(atom_t yy_exp_list, atom_t yy_exp_added)
  {
    auto exp_list = dynamic_cast<ExpList *>(yy_exp_list.NODE);
    this->list.insert(this->list.begin(), exp_list->list.begin(), exp_list->list.end());
    this->list.push_back(yy_exp_added);
  }
  int get_value()
  {
    return list.front().INT;
  }
  TypeEnum get_type()
  {
    return list.front().TYPE;
  }
};
