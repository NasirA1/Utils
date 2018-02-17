#include "leak_detector.h"
#include <cstdio>
#include <string>


struct bar_t
{
  bar_t(): x(new int) { }

  ~bar_t(){ delete x; }

  int* x;
};


void foo()
{
  bar_t bar;
}


int main(int argc, char* argv[])
{
  leak_detector::start_capture();

  char* buff = DBG_NEW char[1024];
  DBG_DELETE(buff);

  struct employee_t { int age; std::string name; double salary; };

  auto emp1 = DBG_NEW employee_t;
  auto emp2 = DBG_NEW employee_t;
  DBG_DELETE(emp2);

  foo();

  auto xs = DBG_NEW int[10];
  delete[] xs;

  leak_detector::dump_findings();
}
