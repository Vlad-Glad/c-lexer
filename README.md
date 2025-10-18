# C-Lexer (Regex-based) — README

A tiny lexical analyzer for the C language, written in C++.
It tokenizes a C source file using **`std::regex`** and prints tokens as:

```
<lexeme, TokenKind, line:column>
```

## Features

* Numbers: decimal integers, hex integers (`0x..`), floats (fraction and/or exponent, optional suffix `f/F/l/L`)
* String and char literals with escapes (reports unterminated ones)
* Preprocessor directives (`#...` with line continuations `\` + newline) as a **single token**
* Comments `// ...` and `/* ... */` (always emitted as tokens)
* Keywords vs identifiers
* Operators and punctuators, including `...`, `#`, `##`, `->`, `<<=`, etc.
* Error tokens for unknown or malformed sequences (with position and short message)

## Requirements

* CMake ≥ 3.16
* C++17 compiler

  * Linux/macOS: GCC/Clang
  * Windows: MSVC (Visual Studio 2022) or MinGW/Clang + Ninja

## Build

### Linux / macOS

```bash
git clone <your-repo-url>
cd c-lexer
mkdir -p build
cmake -S . -B build
cmake --build build
```

### Windows (Visual Studio generator, PowerShell)

```powershell
git clone <your-repo-url>
cd c-lexer
mkdir build
cmake -S . -B build
cmake --build build --config Debug
```

### Windows (Ninja / single-config)

```bash
mkdir -p build
cmake -S . -B build -G "Ninja"
cmake --build build
```

## Run

Pass a path to a C source file.

* Linux/macOS:

```bash
./build/clexer examples/demo.c
# or (if multi-config generator placed the exe inside Debug/Release):
./build/Debug/clexer examples/demo.c
```

* Windows:

```powershell
.\build\Debug\clexer.exe .\examples\demo.c
```

## Output format

Each token is printed on its own line:

```
<lexeme, TokenKind, line:column> [// optional message for errors]
```

Example:

```
<int, Keyword, 1:1>
<x, Identifier, 1:5>
<=, Operator, 1:7>
<42, IntLiteral, 1:9>
<;, Punctuator, 1:11>
<// hello, Comment, 2:3>
<, EOF, 3:1>
```

## Example test files

Use any of these or add your own:

* `examples/demo.c` — mixed sample (macros, comments, numbers, strings)
* Minimal:

  ```c
  int main(void){int x=42; float y=3.5e-1; return x>y;}
  ```
* Errors (to see `Error` tokens):

  ```c
  int main(void){
    char c='A;
    const char* s="hi;
    /* unterminated
    return 0;
  }
  ```

## Project structure

```
include/
  lexer/
    Lexer.hpp
    Token.hpp
src/
  Lexer.cpp
  Token.cpp
  main.cpp
CMakeLists.txt
examples/
  demo.c
```

## Usage from code (library-style)

You can reuse the lexer in your own program:

```cpp
#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
using namespace clex;

Lexer lex(source_code_string);
auto tokens = lex.tokenize();
for (const auto& t : tokens) {
  // t.kind, t.lexeme, t.pos.line/column, t.message (if Error)
}
```

## Notes / Limitations

* This educational implementation uses **ECMAScript**-style `std::regex`.
  If you change patterns, avoid non-ECMAScript features like `(?: )`.
* String/char prefixes (`u8"..."`, `L'a'`, etc.) are not handled; can be added if needed.
* Integer/octal/typed suffix rules are simplified (sufficient for the lab).
* Preprocessor is captured as a single token regardless of indentation.

## Troubleshooting

* **`abort() has been called` / `std::regex_error` early:** usually a bad regex literal.
  Check for unmatched parentheses or unsupported constructs.
* **Program not found:** verify the correct path to the built executable (`build/clexer` vs `build/Debug/clexer.exe`).
* **Non-ASCII paths on Windows:** prefer quoting the path: `"C:\path with spaces\file.c"`.


## Acknowledgments

Prepared for the “System Programming” lab on lexical analysis (C/C++).
