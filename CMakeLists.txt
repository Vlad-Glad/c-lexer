cmake_minimum_required(VERSION 3.16)
project(c_lexer LANGUAGES CXX)

# Встановлюємо стандарт мови
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Бібліотека з лексером (без реалізації — зараз це просто контейнер)
add_library(clex STATIC
  src/Lexer.cpp
)

# Публічні заголовки для користувачів бібліотеки
target_include_directories(clex PUBLIC include)

# Консольна утиліта, що використовує бібліотеку
add_executable(clexer
  src/main.cpp
)

target_link_libraries(clexer PRIVATE clex)