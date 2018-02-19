cmake_minimum_required(VERSION 2.6)
project (app)


include_directories(include)
add_executable(${PROJECT_NAME} src/app.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/../library/include")
link_directories(${PROJECT_BINARY_DIR}/../)
target_link_libraries(${PROJECT_NAME} library)

add_executable(test_${PROJECT_NAME} test/main.cpp)
target_include_directories(test_${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/../library/include")
link_directories(${PROJECT_BINARY_DIR}/../)
target_link_libraries(test_${PROJECT_NAME} library gtest pthread)