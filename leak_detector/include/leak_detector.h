#ifndef LEAK_DETECTOR_LEAK_DETECTOR_H
#define LEAK_DETECTOR_LEAK_DETECTOR_H


#include <cstddef>
#include <new>

#define LEAK_DETECTOR_OVERLOAD_DEFAULTS
//#define LEAK_DETECTOR_IGNORE_UNKNOWNS


void *operator new(size_t, const char *, size_t) throw(std::bad_alloc);
void *operator new[](size_t, const char *, size_t) throw(std::bad_alloc);
void operator delete(void *, const char *, size_t) noexcept;
void operator delete[](void *, const char *, size_t) noexcept;


namespace leak_detector
{
  void start_capture();
  void dump_findings();
  void reset_findings();


  template<class T>
  void DbgDelete(T *o, const char *file, size_t line)
  {
    if (o) {
      o->~T();
      operator delete(static_cast<void *>(o), file, line);
    }
  }
}


#define DBG_NEW         new (__FILE__,__LINE__)
#define DBG_DELETE(p)   leak_detector::DbgDelete(p,  __FILE__, __LINE__)
//#define DBG_DELETE(p) operator delete(p,  __FILE__, __LINE__)


#endif //LEAK_DETECTOR_LEAK_DETECTOR_H
