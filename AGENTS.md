pdp11 or Pluto is a PDP-11/20 Emulator written in C++.

- `documents/` contains the KA11 CPU Manual.
- `src/` is the program source
  - `src/instructions` directory contains implementation of the ISA
- `tests/` directory should contain:
  - All tests as prompted to generate, run and output
  - A `main.cpp` file that compiles the tests
  - A `CMakeLists.txt` to use with CMake and compiling using this
- `llm/` for LLM suggested, associated, produced changes, which can later be merged into
the project root upon obtain permission.

## Work Policy

The Project directory must never directly be modified. Prioritize using `llm/`, and `tests/` directories for producing output, matching with source and testing.

Always work defensively, making sure to document changes and revisit refactoring possibility after major changes.

Prefer running tool calls.