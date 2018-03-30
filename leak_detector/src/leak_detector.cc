#include "leak_detector.h"
#include "get_tid.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include <mutex>
#include <print_call_stack.h>



//Outputs all allocations and deallocations during program execution
//#define LEAK_DETECTOR_LOG_VERBOSE


namespace leak_detector
{
constexpr size_t STACK_BUFFER_SIZE = 10000;

constexpr size_t MAX_PATH = 1024;

constexpr size_t PATH_COLUMN_WIDTH = 40;  //MUST be below MAX_PATH


struct alloc_t
{
  int tid = -1;
  void* ptr = nullptr;
  size_t size = 0lu;
  size_t line = 0lu;
  char file[MAX_PATH] = "unknown";
  size_t count = 0lu;
};



std::array<alloc_t, STACK_BUFFER_SIZE> allocations;
char callstack_buffer[MAX_PATH] = {0};
bool heap_profiling_started = false;
std::mutex mu;


inline char* truncate_left(char* dest, const size_t dest_size, const char* const src, const size_t src_size, const size_t src_count)
{
  if (src_size < dest_size - 1)
    strncpy(dest, src, dest_size - 1);
  else
    strncpy(dest, src + (src_size - src_count), dest_size - 1);

  dest[dest_size - 1] = '\0';
  return dest;
}


void start_capture()
{
  std::lock_guard<std::mutex> guard(mu);
  heap_profiling_started = true;
}


void reset_findings()
{
  std::lock_guard<std::mutex> guard(mu);

  for (auto& entry : allocations)
  {
    entry.ptr = nullptr;
    entry.size = 0lu;
    entry.tid = -1;
    entry.count = 0;
    strcpy(entry.file, "unknown");
  }
  printf("\n\n********** allocations table reset **********\n\n");
}


inline bool is_slot_empty(const int index)
{
  return allocations[index].ptr == nullptr;
}


inline int find_empty_slot()
{
  for (auto i = 0; i < allocations.size(); ++i)
  {
    if (is_slot_empty(i))
      return i;
  }
  return -1;
}


inline int find_slot(const void* const ptr)
{
  for (auto i = 0; i < allocations.size(); ++i)
  {
    if (allocations[i].ptr == ptr)
      return i;
  }
  return -1;
}


inline int find_slot(const char* const file, const int line)
{
  for (auto i = 0; i < allocations.size(); ++i)
  {
    if (!strcmp(allocations[i].file, file) && allocations[i].line == line)
      return i;
  }
  return -1;
}


inline void print_with_callstack(const pid_t tid,
                                 const void* const ptr,
                                 const char* const desc,
                                 const size_t size,
                                 const int callstack_start_index = 3)
{
  s_print_callstack(callstack_buffer, MAX_PATH, callstack_start_index, "          \t                    \t          \t             \t");
  printf("%X\t%20p\t%s\t %12d\t\t%s\n", tid, ptr, desc, static_cast<int>(size), callstack_buffer);
}


int insert(void* const ptr, const size_t size, const int tid, const char* const file, const size_t line)
{
  auto slot = find_slot(file, line);

  if(slot == -1)
    slot = find_empty_slot();

  if (slot == -1)
  {
    mu.unlock();
    dump_findings();
    printf("******************* no more slots left! ************************\n");
    printf("******************* no more slots left! ************************\n");
    printf("******************* no more slots left! ************************\n");
    abort();
  }

  allocations[slot].ptr = ptr;
  allocations[slot].size = size;
  allocations[slot].tid = tid;
  allocations[slot].line = line;
  allocations[slot].count++;
  strncpy(allocations[slot].file, file, MAX_PATH - 1);
  allocations[slot].file[MAX_PATH - 1] = '\0';

  return slot;
}


inline void free_slot_at(const int index)
{
  if (index == -1)
  {
    mu.unlock();
    dump_findings();
    printf("******************* no more slots left! ************************\n");
    printf("******************* no more slots left! ************************\n");
    printf("******************* no more slots left! ************************\n");
    abort();
  }

  if(allocations[index].count <= 1)
  {
    allocations[index].ptr = nullptr;
    allocations[index].size = 0lu;
    allocations[index].tid = -1;
    allocations[index].line = 0;
    allocations[index].count = 0;
    strcpy(allocations[index].file, "unknown");
  }
  else
  {
    allocations[index].count--;
  }
}


void dump_findings()
{
  std::lock_guard<std::mutex> guard(mu);
  auto leak_count = 0;

  printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  HEAP ALLOCATION TABLE DUMP @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

  for (const auto& entry: allocations)
    if (entry.ptr)
    {
      if (entry.line == 0)
        printf("%X\t%20p\t leaked(%d)   \t%13d\t\t%s\n", entry.tid, entry.ptr, entry.count, static_cast<int>(entry.size), entry.file);
      else
        printf("%X\t%20p\t leaked(%d)   \t%13d\t\t%s:%lu\n", entry.tid, entry.ptr, entry.count, static_cast<int>(entry.size), entry.file, entry.line);
      leak_count++;
    }

  if (leak_count < 1)
    printf("                                                   NO LEAKS FOUND!                                              \n");

  printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  END OF HEAP ALLOCATION TABLE DUMP @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
}


inline void dbg_remove_record(const void* const ptr, const char* const file, const size_t line)
{
  if (heap_profiling_started)
  {
#ifdef LEAK_DETECTOR_IGNORE_UNKNOWNS
    if(line == 0)
      return;
#endif

    auto slot = find_slot(file, line);

    if(slot == -1)
      slot = find_slot(ptr);

    if (slot != -1)
    {
#ifdef LEAK_DETECTOR_LOG_VERBOSE
      if (line == 0)
        print_with_callstack(gettid(), allocations[slot].ptr, " freed    ", -allocations[slot].size, 4);
      else
      {
        char short_filename[MAX_PATH];
        truncate_left(short_filename, sizeof(short_filename), file, strlen(file), PATH_COLUMN_WIDTH);
        printf("%X\t%20p\t freed    \t%13d\t\t%s:%d\n", gettid(), allocations[slot].ptr, static_cast<int>(-allocations[slot].size), short_filename, static_cast<int>(line));
      }
#endif
      free_slot_at(slot);
    }
  }
}
}


void* operator new(size_t size, const char* file, size_t line) throw(std::bad_alloc)
{
  using namespace leak_detector;
  std::lock_guard<std::mutex> guard(mu);

  void* ptr = malloc(size);

  if (heap_profiling_started)
  {
    auto tid = gettid();

    if (line == 0)
    {
#ifndef LEAK_DETECTOR_IGNORE_UNKNOWNS
#ifdef LEAK_DETECTOR_LOG_VERBOSE
      print_with_callstack(tid, ptr, " allocated", size);
#endif
      insert(ptr, size, tid, callstack_buffer, line);
#endif
    }
    else
    {
      char short_filename[MAX_PATH];
      truncate_left(short_filename, sizeof(short_filename), file, strlen(file), PATH_COLUMN_WIDTH);
      auto index = insert(ptr, size, tid, short_filename, line);
#ifdef LEAK_DETECTOR_LOG_VERBOSE
      printf("%X\t%20p\t allocated\t %12d\t\t%s:%d\n", tid, ptr, static_cast<int>(size), short_filename, static_cast<int>(line));
#endif
    }
  }

  if (!ptr)
  {
    perror("out of memory!");
    throw std::bad_alloc();
  }

  return ptr;
}


void operator delete(void* ptr, const char* file, size_t line) noexcept
{
  using namespace leak_detector;
  std::lock_guard<std::mutex> guard(mu);

  if (ptr)
  {
    dbg_remove_record(ptr, file, line);
    free(ptr);
    ptr = nullptr;
  }
}


void* operator new[](size_t size, const char* file, size_t line) throw(std::bad_alloc)
{
  return operator new(size, file, line);
}


void operator delete[](void* ptr, const char* file, size_t line) noexcept
{
  operator delete(ptr, file, line);
}


#ifdef LEAK_DETECTOR_OVERLOAD_DEFAULTS


void* operator new(size_t size) throw(std::bad_alloc)
{
  return operator new(size, "unknown", 0);
}


void* operator new[](size_t size) throw(std::bad_alloc)
{
  return operator new[](size, "unknown", 0);
}


void operator delete(void* ptr) noexcept
{
  operator delete(ptr, "unknown", 0);
}


void operator delete[](void* ptr) noexcept
{
  operator delete[](ptr, "unknown", 0);
}


#endif
