cmake_minimum_required(VERSION 3.9)
project(leak_detector)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -rdynamic)


include_directories(include)

set(SOURCE_FILES
  src/print_call_stack.cc
  src/leak_detector.cc
)

add_library(leak_detector STATIC ${SOURCE_FILES})



set(TEST_SOURCES
  test/main.cc
)

add_executable(test_leak_detector ${TEST_SOURCES})
target_link_libraries(test_leak_detector leak_detector gtest pthread)
