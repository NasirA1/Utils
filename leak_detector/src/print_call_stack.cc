#include "print_call_stack.h"
#include <execinfo.h>
#include <cxxabi.h>
#include <cstring>
#include <array>
#include <memory>
#include "uncopyable.h"


template<size_t BUFFERSIZE = 1024lu>
struct name_extractor: uncopyable_t<name_extractor<BUFFERSIZE>>
{
  const char* const operator()(const char* const line)
  {
    const_cast<char*>(extracted)[0] = '\0';

    auto lparen = strstr(line, "(");
    if (lparen)
    {
      auto plus = strstr(lparen, "+");
      if (plus)
      {
        strncpy(const_cast<char*>(extracted), lparen + 1, (plus - lparen) - 1);
        const_cast<char*>(extracted)[plus - lparen - 1] = '\0';
      }
    }

    if (!strlen(extracted))
      strncpy(const_cast<char*>(extracted), line, BUFFERSIZE - 1);

    const_cast<char*>(extracted)[BUFFERSIZE - 1] = '\0';
    return extracted;
  }

  const char extracted[BUFFERSIZE] = {0};
};

struct demangler: uncopyable_t<demangler>
{
  demangler(const char* const mangled)
    : demangled(abi::__cxa_demangle(mangled, NULL, NULL, const_cast<int*>(&status)))
  {
    if (!demangled)
    {
      demangled = const_cast<char*>(mangled);
      owner = false;
    }
  }

  ~demangler()
  { if (owner) free(const_cast<char*>(demangled)); }

  operator char*() const
  { return const_cast<char*>(demangled); }

  operator const char*() const
  { return demangled; }

  const int status = -1;
  char* demangled = nullptr;
  bool owner = true;
};


auto get_callstack_symbols(const int start_index)
{
  using symbols_ptr = std::unique_ptr<char*, void (*)(void*)>;
  using result = std::pair<int, symbols_ptr>;

  std::array<void*, 1024> buff = {0};
  const auto num_calls = backtrace(buff.data(), buff.size());

  if (num_calls <= start_index)
    return result(num_calls, symbols_ptr(nullptr, free));

  return result(num_calls, symbols_ptr(backtrace_symbols(buff.data(), num_calls), free));
}


int print_callstack(const int start_index)
{
  const auto result = get_callstack_symbols(start_index);
  name_extractor<> func_name;

  printf("Callstack:\n");
  for (auto i = start_index; i < result.first - 2; ++i)
  {
    for (auto j = 0; j < i; ++j)
      printf(" ");
    printf(" => %s\n", static_cast<const char*>(demangler(func_name(result.second.get()[i]))));
  }

  return 0;
}


int s_print_callstack(char* s, size_t length, const int start_index, const char* const left_padding)
{
  if (s == nullptr || length == 0)
    return -1;

  const auto result = get_callstack_symbols(start_index);
  name_extractor<> func_name;

  strncpy(s, "Callstack:\n", length - 1);

  for (auto i = start_index; i < result.first - 2; ++i)
  {
    if (left_padding)
      strcat(s, left_padding);
    for (auto j = 0; j < i - start_index; ++j)
    {
      strncat(s, " ", length - 1);
    }
    strncat(s, static_cast<const char*>(demangler(func_name(result.second.get()[i]))), length - 1);
    if (i < result.first - 3)
      strncat(s, "\n", length - 1);
  }

  s[length - 1] = '\0';
  return 0;
}
