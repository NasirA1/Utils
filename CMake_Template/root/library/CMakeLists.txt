cmake_minimum_required(VERSION 2.6)
project (library)

include_directories(include)

add_library(${PROJECT_NAME} STATIC src/hello.cpp)


add_executable(test_${PROJECT_NAME} test/main.cpp)
link_directories(${PROJECT_BINARY_DIR}/../)
target_link_libraries(test_${PROJECT_NAME} library gtest pthread)