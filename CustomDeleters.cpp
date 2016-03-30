#include <iostream>
#include <vector>
#include <memory>

/*
 Requirements:
 You require different types of objects be added to a collection for polymorphic invokation. Some of the objects may be 
 stack-allocated and some heap-allocated.  You want memory to be managed automatically via RAII whilst observing ownership 
 rules - i.e. delete owned objects and omit deletion of non-owned ones, e.g. stack-allocated objects.

 Solution:
 C++11 std::unique_ptr<> with custom deleters can be used to achieve this, as follows.
*/


//Polymorphic interface
struct Animal
{
  virtual void Speak() const = 0;
  virtual ~Animal() {}

  typedef void(*Deleter) (Animal*);
  using ptr = std::unique_ptr<Animal, Deleter>;
};

//Custom deleter lambdas
namespace AnimalDeleters
{
  auto Delete = [](Animal* p) { delete p; };
  auto NoDelete = [](Animal*) {};
}

//Concrete type1 - Cat
struct Cat : public Animal
{
  void Speak() const override
  {
    std::cout << "Cat::Speak() => meow!" << std::endl;
  }

  ~Cat() override
  {
    std::cout << "Cat destroyed" << std::endl;
  }
};

//Concrete type2 - Dog
struct Dog : public Animal
{
  void Speak() const override
  {
    std::cout << "Dog::Speak() => woof!" << std::endl;
  }

  ~Dog() override
  {
    std::cout << "Dog destroyed" << std::endl;
  }
};


struct Zoo
{
  /* Non-owner - delete will not be called on the animal being added */
  void AddAnimal(Animal& animal)
  {
    animals.push_back(Animal::ptr(&animal, AnimalDeleters::NoDelete));
  }

  /* Owner - delete will be called on the animal being created/added */
  template<class AnimalType>
  void AddAnimal()
  {
    animals.push_back(Animal::ptr(new AnimalType, AnimalDeleters::Delete));
  }

  //Polymorphic calls on all animals
  void SpeakAll() const
  {
    for (auto& animal : animals)
      animal->Speak();
  }

private:
  //Container for polymorphic objects
  std::vector<Animal::ptr> animals;
};


int main()
{
  {
    Zoo zoo;

    //Stack allocated animals
    Cat cat;
    Dog dog;
    zoo.AddAnimal(cat);
    zoo.AddAnimal(dog);

    //Heap allocated animals
    zoo.AddAnimal<Cat>();
    zoo.AddAnimal<Dog>();

    //Polymorphic call
    zoo.SpeakAll();

  } //End of scope. All animals will be automatically destroyed after this point


  std::cin.get(); //pause
}
