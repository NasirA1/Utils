#ifndef LEAK_DETECTOR_PRINT_STACK_H
#define LEAK_DETECTOR_PRINT_STACK_H

#include <cstddef>

int print_callstack();
int s_print_callstack(char* s, const size_t length, const int start_index = 2, const char* const left_padding = nullptr);


#endif //LEAK_DETECTOR_PRINT_STACK_H
