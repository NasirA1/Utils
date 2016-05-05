#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <windows.h>

//Write generic string functions that work on both std::string and std::wstring


using namespace std;

template<class CHARTYPE>
inline static basic_string<CHARTYPE>& triml(basic_string<CHARTYPE>& s)
{
  s.erase(0, s.find_first_not_of(' '));
  return s;
}

template<class CHARTYPE>
inline static basic_string<CHARTYPE>& trimr(basic_string<CHARTYPE>& s)
{
  s.erase(s.find_last_not_of(' ') + 1, s.size());
  return s;
}

template<class CHARTYPE>
inline static basic_string<CHARTYPE>& trim(basic_string<CHARTYPE>& s)
{
  return trimr(triml(s));
}


//Helper function for demo purposes
std::string to_utf8(const std::wstring& str)
{
  auto char_count = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
  if (char_count == 0) return "";

  string result;
  result.resize(char_count);
  ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), const_cast<char*>(result.c_str()), char_count, NULL, NULL);

  return result;
}


int main()
{
  ofstream out("out.txt");

  out << '<' << trim("  - Nasir   AHMAD-    "s) << '>' << '\n';
  out << '<' << to_utf8(trim(L"  - نثار احمد-    "s))  << '>' << '\n';

  return 0;
}
