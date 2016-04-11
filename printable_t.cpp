#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;


namespace base
{
  //printable interface
  struct printable_t
  {
    template<typename T>
    printable_t(T obj) : self_(make_shared<model<T>>(move(obj))) {}

    friend void print(const printable_t& obj)
    {
      obj.self_->print_();
    }

  private:
    struct concept_t {
      virtual ~concept_t() = default;
      virtual void print_() const = 0;
    };
    template<typename T>
    struct model : concept_t {
      model(T x) : data_(move(x)) { }
      virtual void print_() const override {
        print(data_);
      }
      T data_;
    };

    shared_ptr<const concept_t> self_;

  };

  //primitive types
  template<typename T>
  void print(T x)
  {
    cout << "print() primitive: => " << x << endl;
  }

  //polymorphic
  void print(const vector<printable_t>& objects)
  {
    for (const auto& obj : objects)
      print(obj);
  }
}


struct CD { 
  CD(int id) : id_(id) {}

  //polymorphic function
  friend void print(const CD& cd)
  {
    cout << "I'm CD" << cd.id_ << endl;
  }

private: 
  int id_; 
};

struct DVD { 
  DVD(int id) : id_(id) {} 

  //polymorphic function
  friend void print(const DVD& dvd)
  {
    cout << "I'm DVD" << dvd.id_ << endl;
  }

private: 
  int id_; 
};





int main()
{
  using namespace base;

  vector<printable_t> stuff;
  stuff.push_back(CD(1));
  stuff.push_back(10);
  stuff.push_back(DVD(2));

  print(stuff);

#ifdef _DEBUG
  getchar();
#endif
  return 0;
}

