#include <iostream>

using namespace std;



template<class T, size_t SIZE>
class ring_buffer_t
{
  T buff[SIZE];
  int front;
  int back;
  int current;

public:
  ring_buffer_t()
    : front(0), back(0), current(0)
  {
    memset(buff, 0, SIZE * sizeof(T));
  }


  void push_back(T value)
  {
    buff[back] = value;
    back = (back + 1) % SIZE;

    if (current < SIZE)
    {
      current++;
    }
    else 
    {
      /* Overwriting the oldest. Move front to next-oldest */
      front = (front + 1) % SIZE;
    }
  }


  T pop_front()
  {
    if (!current)
      throw runtime_error("ERROR: buffer empty!");

    T value = buff[front];
    front = (front + 1) % SIZE;

    current--;
    return value;
  }
};




//ring buffer of size 3 (will hold 3 latest items)
ring_buffer_t<int, 3> buff;


int main(int argc, char* argv[])
{
  buff.push_back(1);
  buff.push_back(2);
  buff.push_back(3);
  buff.push_back(4);
  buff.push_back(5);

  //Will output 3 latest items (i.e. 3, 4 & 5)
  cout << buff.pop_front() << endl;
  cout << buff.pop_front() << endl;
  cout << buff.pop_front() << endl;

  cout << "All done!\n";
  cin.get();
  return EXIT_SUCCESS;
}
