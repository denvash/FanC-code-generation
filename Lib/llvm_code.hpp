#include <string>
using namespace std;

/* exit.ll */
static const string exit_llvm = R"(
declare void @exit (i32))";

/* printf.ll */
static const string printf_llvm = R"(
declare i32 @printf (i8*, ...))";

/* zero_div.ll */
static const string zero_div_llvm = R"(
@.zero_div = constant [24 x i8] c"Error division by zero\0A\00")";

/* print.ll */
static const string print_llvm = R"(
@.str_specifier = constant [4 x i8] c "%s\0A\00"
define void @print (i8 *) {
  call i32(i8 *, ...) @printf(i8 * getelementptr([4 x i8], [4 x i8] * @.str_specifier, i32 0, i32 0), i8 * %0)
  ret void
})";

/* printi.ll */
static const string printi_llvm = R"(
@.int_specifier = constant [4 x i8] c"%d\0A\00"
define void @printi (i32) {
  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)
  ret void
})";

static const string branch_to_bp_llvm = R"(br label @)";
static const string ret_void_llvm = R"(ret void)";
static const string ret_success_llvm = R"(ret i32 0)";
static const string scope_end_llvm = R"(})";

string call_print_llvm(string len, string id)
{
  /* call void @print (i8* getelementptr ([13 x i8], [13 x i8]* @msg, i32 0, i32 0)) */
  return "call void @print (i8* getelementptr ([" + len + " x i8], [" + len + " x i8]* " + id + ", i32 0, i32 0))";
}

string define_func_llvm(bool is_void_type, int size, string id)
{
  string type = is_void_type ? "void" : "i32";
  string args = "";
  for (auto i = 0; i < size; i++)
  {
    args = args + "i32 ,";
  }
  args = args.substr(0, args.length() - 1);
  return "define " + type + " @" + id + "(" + args + ") {\n\
entry:\n\
%fp = alloca [50 x i32]\n";
}

string declare_var_llvm(string var, string size, int offset)
{
  return var + (offset >= 0 ? " = getelementptr [50 x i32], [50 x i32]* %fp, i32 0 , i32 " + to_string(offset) : " = getelementptr [" + size + " x i32], [" + size + " x i32]* %args, i32 0 , i32 " + to_string(((offset + 1) * (-1))));
}

string define_args_llvm(string size)
{
  return "%args = alloca [" + size + " x i32]";
}

string store_arg_llvm(string id, string var)
{
  return " store i32 %" + id + " , i32*" + var;
}

string store_string_llvm(string id, string len, string value)
{
  return id + " = constant [" + len + " x i8] c\"" + value + "\\00\"";
}