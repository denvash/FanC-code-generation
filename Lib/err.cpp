#include "err.hpp"

void err(void_type callback)
{
  callback();
  exit(1);
}

void err(line_number callback, int lineno)
{
  callback(lineno);
  exit(1);
}

void err(line_number_id callback, int lineno, const string &str)
{
  callback(lineno, str);
  exit(1);
}

void err(line_number_id_types callback, int lineno, const string &str, vector<string> &argTypes)
{
  callback(lineno, str, argTypes);
  exit(1);
}
