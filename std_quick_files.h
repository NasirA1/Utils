#pragma once
#include <string>
#include <fstream>
#include <vector>

///Generic way of reading text files (works for both ASCII and UTF8 strings)

using namespace std;

namespace util
{
  
	//Reads all text from the given text file at once and returns result in a string
  template<typename TChar = char>
  auto readAllText(const basic_string<TChar>& filename)
  {
    basic_ifstream<TChar> file(filename);
    basic_string<TChar> str;

    file.seekg(0, ios::end);
    str.reserve(static_cast<size_t>(file.tellg()));

    file.seekg(0, ios::beg);
    str.assign((istreambuf_iterator<TChar>(file)), istreambuf_iterator<TChar>());
    
    return str;
  }


	//Reads all text from the given text file line by line and returns result in a vectror of strings
  template<typename TChar = char>
  auto readAllLines(const basic_string<TChar>& filename)
  {
    vector<basic_string<TChar>> lines;
    basic_ifstream<TChar> file(filename);
    
    while (!file.eof())
    {
      basic_string<TChar> line;
      getline(file, line);
      lines.emplace_back(line);
    }

    return lines;
  }


	//Reads all text from the given text file at once and returns result in a string
  template<typename TChar = char>
  auto readAllText(const TChar* const filename)
  {
    return readAllText(basic_string<TChar>(filename));
  }


	//Reads all text from the given text file line by line and returns result in a vectror of strings
  template<typename TChar = char>
  auto readAllLines(const TChar* const filename)
  {
    return readAllLines(basic_string<TChar>(filename));
  }

}
