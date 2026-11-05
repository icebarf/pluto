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

The executable is the useful form while failures exist because it prints the
failing assertion and the actual/expected values.

## Current result

```text
217 passed, 5 failed, 222 total (498 checks)
```

Only the JMP autoincrement special case, JSR, and RTI remain. Root causes and suggested fixes are documented in
[`llm/remaining_instruction_test_findings.md`](../llm/remaining_instruction_test_findings.md).

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
| CMP | 63 | 6 | 6 | borrow cases pass; CMPB is not implemented |
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
| JMP | 106 | 6 | 5 |
| JSR | 91-92 | 3 | 0 |
| RTS | 90 | 2 | 2 |
| EMT, TRAP, BPT, IOT | 93-97 | 4 | 4 |
| RTI | 105 | 1 | 0 |
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

## Known failures and handbook references

| Failing group | Count | Handbook | Cause |
| --- | ---: | ---: | --- |
| JMP | 1 | 106 | normal effective-address modes pass; mode-2 (autoincrement) must increment before use |
| JSR | 3 | 91-92 | destination still treated as an absolute low-5-bit address and read as a pointer |
| RTI | 1 | 105 | SP is decremented instead of incremented while popping |

Fixes verified in this pass: BGT condition, trap vector literals, the
ADD/SUB/CMP unsigned carry/borrow calculations, and JMP's normal
effective-address handling. The `sub_word_negative_overflow` test expectation
was corrected to include the borrow flag.

Suggested fixes for the remaining three instructions are in
[`llm/remaining_instruction_test_findings.md`](../llm/remaining_instruction_test_findings.md).
Applying those three changes was validated in a scratch copy: **222/222 pass**.

## Deliberately excluded

- `WAIT`, `HALT`, `RESET`: unimplemented console-dependent instructions.
- `CMPB`: the current `cmp()` asserts word-only.
- `ADDB`, `SUBB`: word-only on the 11/20 in the current implementation.
- `RTT`, `MARK`, `MFPI`, `MTPI`, `SPL`: not present in the current source.
