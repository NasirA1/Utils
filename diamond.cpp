#include <iostream>

using namespace std;


struct Grandfather
{
  virtual void foo() { }
};

struct Father : /*virtual*/ Grandfather
{
  void foo() override { }
};

struct Uncle : /*virtual*/ Grandfather
{
};

struct Son : Father, Uncle
{
};



int main()
{
  Son son;
  son.foo(); //this call is amigious and won't compile.  The compiler doesn't know whether to call Grandfather::foo()
  //inherited via Uncle or Father::foo() inherited from Father.  The solution is to uncomment the virtual keyword.
  //Son will then inherit the foo() from Father via 'dominance' rule.

#if _DEBUG
  cin.get();
#endif
  return 0;
}
