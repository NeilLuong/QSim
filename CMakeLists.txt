cmake_minimum_required(VERSION 3.16)
project(QSim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(qsim src/main.cpp src/Parser.cpp src/StateVector.cpp)
target_include_directories(qsim PRIVATE include)

# Run with: ctest --test-dir build --output-on-failure
enable_testing()
add_test(NAME version1 COMMAND qsim ${CMAKE_SOURCE_DIR}/test/version1.txt)
add_test(NAME version2 COMMAND qsim ${CMAKE_SOURCE_DIR}/test/version2.txt)
add_test(NAME version3 COMMAND qsim ${CMAKE_SOURCE_DIR}/test/version3.txt)
