# AGENTS.md

## Project Conventions

- Use `snake_case` for file names.
- Use `camelCase` for C variable and function names.
- Use `UPPER_SNAKE_CASE` for `#define` constants.
- Prefer tagged unions for variant data modeling in C.

## CMake

- C standard is set to C23.
- CMake minimum version is set to 3.21 to support C23 settings.
- Keep C extensions disabled (`CMAKE_C_EXTENSIONS OFF`).

## Verification

- Build command: `cmake --build build -j`
- Run sample: `./build/sandbox`
