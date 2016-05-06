#include <iostream>
#include <sstream>
#include <fstream>

#include <chrono>  //system_clock
#include <iomanip> //put_time


using namespace std;
using namespace chrono;


struct current_time
{
  template<class OUTSTREAM>
  friend OUTSTREAM& operator<<(OUTSTREAM& out, current_time&)
  {
    auto now = system_clock::to_time_t(system_clock::now());
    out << put_time(localtime(&now), "%F %T");
    return out;
  }
};



int main()
{
  //output to stdout
  cout << current_time() << endl;

  //output to string
  ostringstream oss;
  oss << current_time();
  cout << oss.str() << endl;

  //output to file
  ofstream file("test.txt");
  file << current_time() << endl;

#if _DEBUG
  cin.get();
#endif
  return 0;
}
