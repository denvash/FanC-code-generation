#include "register_handler.hpp"
#include "bp.hpp"

// Fix to infinite
#define NUM_OF_REGISTERS 18

typedef map<string, int>::iterator reg_it;
typedef map<string, int>::reverse_iterator reg_rit;

static const string register_lut[] = {
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9"};

static const string division_by_zero_error("\"Error division by zero\n\"");

priority_queue<int, vector<int>, std::greater<int>> RegisterHandler::available = priority_queue<int, vector<int>, std::greater<int>>();
map<string, int> RegisterHandler::busy = map<string, int>();

RegisterHandler::RegisterHandler() {}
RegisterHandler::RegisterHandler(bool allocate)
{
  if (allocate)
  {
    reg_str = register_allocate();
    reg_id = busy.at(reg_str);
  }
}

void RegisterHandler::init()
{
  for (int i = 0; i < NUM_OF_REGISTERS; i++)
    available.push(i);
}

string RegisterHandler::register_allocate()
{
  auto reg_idx = available.top();
  available.pop();
  auto reg = register_lut[reg_idx];
  busy[reg] = reg_idx;

  return reg;
}

void RegisterHandler::register_free(string reg)
{
  if (busy.find(reg) == busy.end())
  {
    return;
    cout << "Error: register " << reg << "not in map" << endl;
    exit(-1);
  }
  auto reg_idx = busy.at(reg);
  busy.erase(reg);
  available.push(reg_idx);
}

void RegisterHandler::release_all()
{
  for (reg_it it = busy.begin(); it != busy.end(); it++)
  {
    register_free(register_lut[it->second]);
  }
}

map<string, int> &RegisterHandler::get_busy_registers()
{
  return busy;
}