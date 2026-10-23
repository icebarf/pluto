# PDP-11/20 single-operand instruction tests

These tests exercise every single-operand instruction implemented in
`src/instructions/`: CLR/CLRB, COM/COMB, INC/INCB, DEC/DECB, NEG/NEGB,
TST/TSTB, ASL/ASLB, ASR/ASRB, ROL/ROLB, ROR/RORB, ADC/ADCB, SBC/SBCB and
SWAB. They also cover the addressing modes and the operand resolution
helpers that the instructions use.

The project's `src/ka11.cpp` is not a runnable CPU yet, so `test_framework.hpp`
provides a small `TestMachine` that places one instruction at `01000`, sets PC
to `01002` (simulating the instruction fetch), and calls the instruction
implementation directly through `Instructions::extract()`.

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

`ctest` summarises only pass/fail; run the executable directly to see the
individual failing assertions and their values.

## Layout

| File | Purpose |
| --- | --- |
| `test_framework.hpp` | Minimal test registry, assertions, `TestMachine`, instruction encoder |
| `main.cpp` | Runs all registered tests and prints a checkmark per test |
| `test_extract.cpp` | Decode of the single-operand format (and SUB/MOVB classification guards) |
| `test_clr.cpp` | CLR/CLRB |
| `test_com.cpp` | COM/COMB |
| `test_inc_dec.cpp` | INC/INCB and DEC/DECB |
| `test_neg.cpp` | NEG/NEGB |
| `test_tst.cpp` | TST/TSTB |
| `test_adc_sbc.cpp` | ADC/ADCB and SBC/SBCB |
| `test_shifts.cpp` | ASL/ASLB, ASR/ASRB, ROL/ROLB, ROR/RORB |
| `test_swab.cpp` | SWAB |
| `test_addressing.cpp` | Byte/word step sizes, SP/PC rules, deferred modes, index PC handling |
| `test_operand.cpp` | `resolve_operand` / `read_operand` / `write_operand` semantics |

Tests use `CODE = 01000`, `DATA = 02000` and `TARGET = 03000` so that test data
cannot overlap the instruction word. `execute()` writes the instruction at
`01000`, the optional index/immediate word at `01002`, then sets PC to `01002`
before calling the implementation.

## Current result

```text
103 passed, 19 failed, 122 total
```

All 19 failures correspond to implementation issues, not test errors. They are
listed in [Known failures](#known-failures) below.

## ISA coverage

The condition-code expectations are taken from the 1972 PDP-11/20 Handbook in
`documents/`.

| Instruction | Handbook page | Tests | Covered |
| --- | ---: | ---: | --- |
| CLR / CLRB | 42 | 12 | Word/byte register and memory; N=0 Z=1 V=0 C=0; register high byte preserved; autoincrement, autodecrement, deferred, index, absolute, relative, relative-deferred, immediate |
| COM / COMB | 47 | 7 | Word/byte; N/Z from result, V=0, C=1; byte high byte preserved; memory word/byte |
| INC / INCB | 44 | 8 | N/Z; V only at old `077777` / `0177`; C unaffected; wrap; byte step 1; autoincrement/autodecrement/index |
| DEC / DECB | 43 | 8 | N/Z; V only at old `100000` / `0200`; C unaffected; wrap; byte step 1 |
| NEG / NEGB | 45 | 8 | N/Z; V at result `100000` / `0200`; C = result != 0; byte high byte preserved; memory autoincrement |
| TST / TSTB | 46 | 6 | N/Z from operand, V=0 C=0; no write; byte zero ignores high byte |
| ADC / ADCB | 52 | 8 | C=0 retains destination; C=1 adds one; C out at `177777`/`0377`; V at `077777`/`0177` when C=1; N/Z |
| SBC / SBCB | 53 | 10 | C=0 retains destination; C=1 subtracts one; result 0 clears C; borrow; V at result `100000`/`0200`; N/Z |
| ASL / ASLB | 50 | 6 | C = old bit 15/7; V = N xor C; N/Z; byte high byte preserved |
| ASR / ASRB | 49 | 7 | C = old bit 0; sign replication; V = N xor C; N/Z; byte high byte preserved |
| ROL / ROLB | 55 | 6 | C = old bit 15/7; old C into bit 0/7; V = N xor C; N/Z |
| ROR / RORB | 56 | 6 | C = old bit 0; old C into bit 15/7; V = N xor C; N/Z |
| SWAB | 57 | 6 | Byte swap; N = result bit 7; Z = low byte of result zero; V=0 C=0; register and memory |
| Addressing | 22–35 | 10 | Byte/word step sizes; SP/PC always 2; deferred autoincrement/decrement always 2; index consumes a word and advances PC; side effects happen once; register-deferred does not modify its register |
| Operand helpers | 3.3, 3.4 | 6 | `resolve` applies side effects once; `read`/`write` are pure; byte/SP/index rules |
| Decode | 3.1, 3.2 | 8 | Single-operand opcode/mode/register/byte bit; SWAB; SUB is double word (not byte); MOVB is double byte; EIS is not double |

### Condition-code matrix used by the tests

For byte operations, “negative” is bit 7 and “zero” is the low byte being zero.

| Instruction | N | Z | V | C |
| --- | --- | --- | --- | --- |
| CLR / CLRB | 0 | 1 | 0 | 0 |
| COM / COMB | result < 0 | result = 0 | 0 | 1 |
| INC / INCB | result < 0 | result = 0 | old = `077777` / `0177` | unaffected |
| DEC / DECB | result < 0 | result = 0 | old = `100000` / `0200` | unaffected |
| NEG / NEGB | result < 0 | result = 0 | result = `100000` / `0200` | result != 0 |
| TST / TSTB | operand < 0 | operand = 0 | 0 | 0 |
| ADC / ADCB | result < 0 | result = 0 | old = `077777`/`0177` and C=1 | old = `177777`/`0377` and C=1 |
| SBC / SBCB | result < 0 | result = 0 | result = `100000` / `0200` | 0 iff result = 0 and C was 1 |
| ASL / ASLB | result < 0 | result = 0 | N xor C | old bit 15 / 7 |
| ASR / ASRB | result < 0 | result = 0 | N xor C | old bit 0 |
| ROL / ROLB | result < 0 | result = 0 | N xor C | old bit 15 / 7 |
| ROR / RORB | result < 0 | result = 0 | N xor C | old bit 0 |
| SWAB | result bit 7 | low byte = 0 | 0 | 0 |

## Known failures

These are assertions that disagree with the current source. They are grouped by
root cause; the suggested minimal fixes are in
`llm/single_operand_test_findings.md`.

### 1. `write_operand` writes memory for register operands — 3 tests

Failing:

- `clr_register_does_not_write_memory`
- `register_word_write_does_not_touch_memory`
- `register_byte_write_does_not_touch_memory`

`write_operand()` writes the register and then falls through to the memory
write at `operand.address` (which is `0` for a resolved register operand).
Every register-mode instruction therefore also writes memory location 0.
This is not observable in the instruction tests that only inspect registers,
but the dedicated tests check the sentinel at address 0.

### 2. NEG/NEGB carry is inverted — 8 tests

Failing:

- `neg_word_example`, `neg_word_zero`, `neg_word_two`, `neg_word_most_negative`
- `neg_byte_register_preserves_high_byte`, `neg_byte_zero`, `neg_byte_most_negative`
- `neg_word_autoincrement`

Handbook page 45: “C: cleared if the result is 0; set otherwise.”
Current code sets C when the result is zero and leaves C unchanged otherwise.
All other NEG flags (N/Z/V and the two's-complement result) pass.

### 3. ADC/ADCB — 5 tests

Failing:

- `adc_word_carry_clear_retains_value`
- `adc_byte_carry_clear_retains_value`
- `adc_word_overflow`
- `adc_byte_carry_out`
- `adc_byte_overflow`

With C=0 the destination must be unchanged; the current code initialises the
result to 0 and only assigns `oldvalue + 1` when C=1.

The V and byte-C boundaries are also wrong:

- V must use the incoming C and `oldvalue == 077777` (word) or `0177` (byte).
  The current code tests the result and reads C after it has already been
  updated.
- Byte C must compare `oldvalue & 0377` with `0377`; the current code compares
  the whole word with `0177777`.

### 4. SBC/SBCB with C=0 — 2 tests

Failing:

- `sbc_word_carry_clear_retains_value`
- `sbc_byte_carry_clear_retains_value`

Same issue as ADC: the result is initialised to 0 and only assigned
`oldvalue - 1` when C=1. With C=0 the destination must be retained.
The SBC C/V logic for C=1 is correct and all other SBC tests pass.

### 5. SWAB zero flag — 1 test

Failing:

- `swab_zero_low_byte_with_nonzero_high_byte`

Handbook page 57: “Z: set if low-order byte of result = 0”.
Current code tests the entire 16-bit result, so `SWAB` of `000012` (result
`005000`, low byte 0, high byte nonzero) clears Z instead of setting it.
The N definition (bit 7 of the result) is correct and its test passes.

## Notes on the implementation review

- **ADC/SBC with C=0:** the intended behavior is to retain the destination
  (add/subtract zero). The tests assert that. The current source still has
  `value = 0` followed by a conditional assignment, so the tests fail; if a
  fix exists locally, it is not present in the tested working tree.
- **SWAB N:** corrected during review. N is result bit 7, not result bit 15;
  `swab_negative_from_low_byte` now passes.
- **SBC V/C:** left as implemented. The C=1 tests (`sbc_word_carry_set_subtracts_one`,
  `sbc_word_result_zero_clears_carry`, `sbc_word_borrow`, `sbc_word_overflow`
  and the byte equivalents) all pass, so no change is suggested for that path.
