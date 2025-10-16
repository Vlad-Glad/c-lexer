cmake_minimum_required(VERSION 3.16)
project(c_lexer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Бібліотека з лексером
add_library(clex STATIC
  src/Lexer.cpp
 "src/Token.cpp" "example1.c" "example2.c" "example3.c")

# Публічні заголовки для користувачів бібліотеки
target_include_directories(clex PUBLIC include)

# Консольна утиліта, що використовує бібліотеку
add_executable(clexer
  src/main.cpp
 "src/Token.cpp" "example1.c" "example2.c" "example3.c")

target_link_libraries(clexer PRIVATE clex)