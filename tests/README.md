# PDP-11/20 instruction tests

This suite covers every instruction implemented in `src/instructions/` except
the console-dependent `WAIT`, `HALT` and `RESET`, which are intentionally
unimplemented.

Coverage includes:

- all single-operand instructions;
- double-operand MOV/MOVB, ADD, SUB, CMP, BIS/BISB, BIT/BITB, BIC/BICB;
- all conditional and unconditional branches;
- JMP, JSR, RTS, RTI;
- EMT, TRAP, BPT and IOT traps;
- all condition-code operators.

## Build and run

From the project root:

```sh
cmake -S tests -B tests/build
cmake --build tests/build
./tests/build/pdp11_tests
```

One-liner:

```sh
cmake -S tests -B tests/build && cmake --build tests/build && ./tests/build/pdp11_tests
```

CTest is also registered:

```sh
ctest --test-dir tests/build --output-on-failure
```

The executable prints every test name and any failing assertion with actual and
expected values.

## Current result

```text
222 passed, 0 failed, 222 total (498 checks)
```

All covered instructions pass. The findings and fixes from the test-driven work
are consolidated in [`llm/README.md`](../llm/README.md).

## Framework design

The framework in `support/test_framework.hpp` is template-based:

- Instruction descriptors (`op::Mov`, `op::Add`, `branch::Beq`, ...) bind an
  implementation function to its opcode/base. `Machine::execute<T>` resolves
  dispatch at compile time; there is no runtime opcode switch.
- `Machine` provides `execute` (single operand), `execute_double`,
  `execute_branch`, `execute_word` (fixed-word instructions), `execute_jmp`,
  `execute_jsr` and `execute_rts`.
- Tests are registered in a fixed `std::array<TestCase, 512>`; no heap
  allocation and no `std::vector` growth.
- One `Context`/`Machine` is reused for every test; each test resets it.
- Register and branch cases are `constexpr std::array` tables expanded at
  compile time with `std::index_sequence` and fold expressions. Every case is
  still registered as an individually named test.
- Assertions use `std::source_location`, `std::string_view` and `std::span`;
  condition codes use a small `Flags` type with `operator|`.
- `WAIT`, `HALT` and `RESET` are deliberately not tested.

## Layout

```text
tests/
├── CMakeLists.txt
├── main.cpp
├── support/
│   └── test_framework.hpp
├── decode/
│   └── test_extract.cpp
├── addressing/
│   └── test_addressing.cpp
├── operand/
│   └── test_operand.cpp
└── instructions/
    ├── single_operand/
    │   ├── general/              CLR, COM, INC, DEC, NEG, TST
    │   ├── multiple_precision/   ADC, SBC
    │   ├── shift_rotate/         ASL, ASR, ROL, ROR
    │   └── byte_swap/            SWAB
    ├── double_operand/           MOV, ADD, SUB, CMP, BIS/BIT/BIC
    ├── branches/                 BR, BEQ, BNE, BMI, BPL, BCS, BCC,
    │                             BVS, BVC, BLT, BGE, BLE, BGT, BHI, BLOS
    └── control/
        ├── test_jmp.cpp          JMP
        ├── test_jsr_rts.cpp      JSR, RTS
        ├── test_traps.cpp        EMT, TRAP, BPT, IOT, RTI
        └── test_condition_codes.cpp  CLC/SEC ... CCC/SCC
```

`tests/CMakeLists.txt` compiles the instruction sources directly. It excludes
`src/main.cpp`, `wait.cpp`, `halt.cpp` and `reset.cpp`.

## Coverage

### Single operand

| Instruction | Handbook | Tests | Passing |
| --- | ---: | ---: | ---: |
| CLR/CLRB, COM/COMB, INC/INCB, DEC/DECB, NEG/NEGB, TST/TSTB | 42-47 | 49 | 49 |
| ADC/ADCB, SBC/SBCB | 52-53 | 18 | 18 |
| ASL/ASLB, ASR/ASRB, ROL/ROLB, ROR/RORB | 49-56 | 25 | 25 |
| SWAB | 57 | 6 | 6 |
| Addressing-mode side effects | 22-35 | 10 | 10 |
| Operand resolution | 3.3, 3.4 | 6 | 6 |
| Decode | 3.1, 3.2 | 8 | 8 |

### Double operand

| Instruction | Handbook | Tests | Passing | Notes |
| --- | ---: | ---: | ---: | --- |
| MOV/MOVB | 59-60 | 8 | 8 | register, immediate, byte sign-extension, memory-to-memory, both autoincrement |
| ADD | 61 | 6 | 6 | carry-out cases pass |
| SUB | 62 | 6 | 6 | borrow cases pass |
| CMP | 63 | 6 | 6 | borrow cases pass; CMPB does not exist on the 11/20 |
| BIS/BISB, BIT/BITB, BIC/BICB | 65-67 | 8 | 8 | C unaffected, V cleared, byte forms |

### Branches

| Group | Handbook | Tests | Passing |
| --- | ---: | ---: | ---: |
| BR | 69 | 2 | 2 |
| BEQ, BNE, BMI, BPL, BCS, BCC, BVS, BVC | 71-78 | 17 | 17 |
| BLT, BGE, BLE | 80-82 | 9 | 9 |
| BGT | 83 | 5 | 5 |
| BHI, BLOS | 85-86 | 6 | 6 |

Every branch test checks both PC and that the condition codes are unchanged.

### Control, traps and condition codes

| Group | Handbook | Tests | Passing |
| --- | ---: | ---: | ---: |
| JMP | 106 | 6 | 6 |
| JSR | 91-92 | 3 | 3 |
| RTS | 90 | 2 | 2 |
| EMT, TRAP, BPT, IOT | 93-97 | 4 | 4 |
| RTI | 105 | 1 | 1 |
| CLC/SEC, CLV/SEV, CLZ/SEZ, CLN/SEN | 107-108 | 8 | 8 |
| CLVC, CCC, SCC | 107-108 | 3 | 3 |

## Condition-code reference

For byte operations "negative" is bit 7 and "zero" is the low byte being zero.

| Instruction | N | Z | V | C |
| --- | --- | --- | --- | --- |
| CLR | 0 | 1 | 0 | 0 |
| COM | result < 0 | result = 0 | 0 | 1 |
| INC / DEC | result < 0 | result = 0 | old boundary | unaffected |
| NEG | result < 0 | result = 0 | result = 100000 | result != 0 |
| TST | operand < 0 | operand = 0 | 0 | 0 |
| ADC | result < 0 | result = 0 | old = 077777 and C=1 | old = 177777 and C=1 |
| SBC | result < 0 | result = 0 | result = 100000 | 0 iff result=0 and C=1 |
| ASL/ASR/ROL/ROR | result < 0 | result = 0 | N xor C | shifted-out bit |
| MOV | src < 0 | src = 0 | 0 | unaffected |
| ADD | result < 0 | result = 0 | signed overflow | carry out |
| SUB | result < 0 | result = 0 | signed overflow | borrow (dst < src) |
| CMP | (src-dst) < 0 | src = dst | signed overflow | borrow (src < dst) |
| BIT | AND < 0 | AND = 0 | 0 | unaffected |
| BIC/BIS | result < 0 | result = 0 | 0 | unaffected |

Branch conditions follow the handbook pages 69-88.

## Test findings

All 222 tests pass. The main issues found and fixed while building this suite
were:

- register writes falling through into memory location 0;
- NEG/NEGB carry inversion;
- ADC/SBC C=0 retention and ADC V/C boundaries;
- SWAB Z defined on the low byte;
- BGT condition;
- trap vectors written as decimal instead of octal;
- ADD/SUB/CMP carry and borrow derived from signed arithmetic;
- JMP effective address and its autoincrement special case;
- JSR destination/linkage handling;
- RTI stack-pop direction.

The consolidated write-up, including handbook page references, is in
[`llm/README.md`](../llm/README.md).

## ISA boundary and exclusions

- `WAIT`, `HALT`, `RESET`: 11/20 instructions, but unimplemented because they
  need the console layer.
- `CMPB`: does not exist on the PDP-11/20; CMP is word-only.
- `ADDB`, `SUBB`: do not exist on the PDP-11/20; ADD and SUB are word-only.
- `RTT`, `MARK`, `MFPI`, `MTPI`, `SPL`: PDP-11/45 instructions, not PDP-11/20.
