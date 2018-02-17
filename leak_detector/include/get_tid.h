#ifndef LEAK_DETECTOR_GET_TID_H
#define LEAK_DETECTOR_GET_TID_H

#include <unistd.h>
#include <sys/syscall.h>


static inline pid_t gettid()
{
  pid_t tid = syscall(SYS_gettid);
  return tid;
}


#endif //LEAK_DETECTOR_GET_TID_H
