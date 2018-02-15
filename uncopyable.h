#ifndef UTIL_UNCOPYABLE_H
#define UTIL_UNCOPYABLE_H
#pragma once


template<class T>
struct uncopyable_t
{
  uncopyable_t(const uncopyable_t&) = delete;
  uncopyable_t& operator= (const uncopyable_t&) = delete;
  uncopyable_t(uncopyable_t&&) = default;
  uncopyable_t&operator= (uncopyable_t&&) = default;
protected:
  uncopyable_t() = default;
  ~uncopyable_t() = default;
};


#endif //UTIL_UNCOPYABLE_H
