#ifndef _REGISTER_HANDLER_HPP
#define _REGISTER_HANDLER_HPP

#include <queue>
#include <string>
#include <map>
#include <iostream>
#include "parser.hpp"

class RegisterHandler
{
private:
  static priority_queue<int, vector<int>, std::greater<int>> available;
  static map<string, int> busy;

  string reg_str;
  int reg_id;

  string register_allocate();
  static void register_free(string reg);

public:
  RegisterHandler();
  RegisterHandler(bool allocate);

  void release()
  {
    register_free(reg_str);
  }

  static void init();
  static void release_all();
  static map<string, int> &get_busy_registers();
  static void new_scope();
  static void restore_scope(vector<int> reg_list);

  friend ostream &operator<<(ostream &os, const RegisterHandler &reg);
  string to_string();
};

#endif