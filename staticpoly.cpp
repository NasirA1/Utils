#include <iostream>

using namespace std;

//dynamic polymorphism won't work below
namespace bad_code
{
  struct Base
  {
    Base() { OnConstruct(); }
    virtual ~Base() { OnDestruct(); }

    virtual void OnConstruct() { cout << "ERROR: Unexpected call to Base!\n"; }
    virtual void OnDestruct() { cout << "ERROR: Unexpected call to Base!\n"; }
  };

  struct Foo : public Base
  {
    virtual void OnConstruct() override
    {
      cout << "Foo Created\n";
    }

    virtual void OnDestruct() override
    {
      cout << "Foo Destroyed\n";
    }
  };

  struct Bar : public Base
  {
    virtual void OnConstruct() override
    {
      cout << "Bar Created\n";
    }

    virtual void OnDestruct() override
    {
      cout << "Bar Destroyed\n";
    }
  };
}


//static polymorphism implementation using CRTP
namespace good_code
{
  template<class DERIVED>
  struct Base
  {
    //WARNING: OnConstruct is called BEFORE the derived class's construction
    Base()
    {
      static_cast<DERIVED&>(*this).OnConstruct(
        static_cast<DERIVED&>(*this)
        );
    }

    ~Base()
    {
      static_cast<DERIVED&>(*this).OnDestruct(
        static_cast<DERIVED&>(*this)
        );
    }
  };


  struct Foo : public Base<Foo>
  {
    static void OnConstruct(Foo& self)
    {
      cout << "Foo Being Created\n";
    }

    static void OnDestruct(Foo& self)
    {
      cout << "Foo Destroyed\n";
    }
  };

  struct Bar : public Base<Bar>
  {
    static void OnConstruct(Bar& self)
    {
      cout << "Bar Being Created\n";
    }

    static void OnDestruct(Bar& self)
    {
      cout << "Bar Destroyed\n";
    }
  };
}


int main()
{
  cout << "bad code:\n";
  {
    bad_code::Foo foo;
    bad_code::Bar bar;
  }
  cout << endl;

  cout << "good code:\n";
  {
    good_code::Foo foo;
    good_code::Bar bar;
  }
  cout << endl;

  cin.get();
  return 0;
}
