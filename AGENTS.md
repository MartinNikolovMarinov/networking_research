# AGENTS.md

## Project Conventions

- Use `snake_case` for file names.
- Use `camelCase` for C variable and function names.
- Use `UPPER_SNAKE_CASE` for `#define` constants.
- Prefer tagged unions for variant data modeling in C.
- Do not use complex one-line return expressions; split calculations into readable intermediate variables.
- Prefer explicit return locals for non-trivial returns:
  - `uint64_t ret = ...;`
  - `return ret;`

## Project Namespacing

- Use `nr` as the mandatory public symbol prefix for all project-owned C APIs.
- Public function names must be `nr` + `camelCase`, for example: `nrTestRunnerInit`.
- Public type names must be `Nr` + PascalCase, for example: `NrTestRunner`.
- Public enum names and values must be prefixed with `Nr` / `NR_` to avoid collisions.
- Public macro/constants must use `NR_` upper snake case.
- Public global variables are disallowed unless explicitly justified; if needed they must use `nr` prefix.
- Internal (file-local) symbols must be `static` and may use plain `camelCase`.
- Project file names must use `nr_` prefix plus snake case suffix, for example: `nr_time.c`.
- Exception: files under `tests/` should not use the `nr_` filename prefix.

## CMake

- C standard is set to C23.
- CMake minimum version is set to 3.21 to support C23 settings.
- Keep C extensions disabled (`CMAKE_C_EXTENSIONS OFF`).

## Verification

- Build command: `cmake --build build -j`
- Run tests:
  - `./build/tests`
- Optional one-shot configure + build + test:
  - `cmake -S . -B build && cmake --build build -j && ./build/tests`
- Run sample: `./build/sandbox`
- Agents must run `./build/tests` after code changes and before claiming the task is ready.
- If tests fail or cannot run, agents must report that clearly in the final response.
