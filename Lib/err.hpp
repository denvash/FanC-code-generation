#ifndef ERR_HPP
#define ERR_HPP

#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

typedef void (*void_type)(void);
typedef void (*line_number)(int);
typedef void (*line_number_id)(int, const string &);
typedef void (*line_number_id_types)(int, const string &, vector<string> &);

void err(void_type callback);
void err(line_number callback, int lineno);
void err(line_number_id callback, int lineno, const string &str);
void err(line_number_id_types callback, int lineno, const string &str, vector<string> &argTypes);

#endif /* ERR_HPP */