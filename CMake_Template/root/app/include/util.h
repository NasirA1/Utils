#ifndef ROOT_UTIL_H
#define ROOT_UTIL_H

template<typename T1, typename T2>
auto add(const T1& a, const T2& b) -> decltype(a + b)
{
  return a + b;
}

#endif //ROOT_UTIL_H
