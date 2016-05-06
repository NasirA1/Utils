#include <iostream>
#include <type_traits>


using namespace std;


//byte-swappable trait
template <typename T>
struct is_byteswapable {
  static const bool value = std::is_integral<T>::value && sizeof(T) >= 2;
};


//function that swaps bytes of the input (e.g. 0x1122 returns 0x2211)
//inputs must be integral and bigger than 2 bytes
template <typename T>
T byte_swap(T value) 
{
  static_assert(is_byteswapable<T>::value, "Cannot swap values of this type");

  unsigned char* bytes = reinterpret_cast<unsigned char*>(&value);

  for (size_t i = 0; i < sizeof(T); i += 2) 
  {
    unsigned char v = bytes[i];
    bytes[i] = bytes[i + 1];
    bytes[i + 1] = v;
  }

  return value;
}



int main()
{
  //this works - input type is <int>
  auto out = byte_swap(0x11223344);
  cout << hex << out << '\n';

  //this also works - input type is <long long>
  auto out2 = byte_swap(0x1122334455667788);
  cout << hex << out2 << '\n';

  //this fails at compile-time because type <float> is not byte-swappable
  //byte_swap(1.2f);

#if _DEBUG
  cin.get();
#endif
  return 0;
}
