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
118 passed, 4 failed, 122 total
```

The only remaining failures are the four ADC/ADCB flag-boundary tests described
below. All other single-operand instruction, addressing and operand-helper
tests pass.

## ISA coverage

Condition-code expectations are taken from the 1972 PDP-11/20 Handbook in
`documents/`.

| Instruction | Handbook page | Tests | Passing | Covered |
| --- | ---: | ---: | ---: | --- |
| CLR / CLRB | 42 | 12 | 12 | Word/byte register and memory; N=0 Z=1 V=0 C=0; register high byte preserved; autoincrement, autodecrement, deferred, index, absolute, relative, relative-deferred, immediate |
| COM / COMB | 47 | 7 | 7 | Word/byte; N/Z from result, V=0, C=1; byte high byte preserved; memory word/byte |
| INC / INCB | 44 | 8 | 8 | N/Z; V only at old `077777` / `0177`; C unaffected; wrap; byte step 1; autoincrement/autodecrement/index |
| DEC / DECB | 43 | 8 | 8 | N/Z; V only at old `100000` / `0200`; C unaffected; wrap; byte step 1 |
| NEG / NEGB | 45 | 8 | 8 | N/Z; V at result `100000` / `0200`; C = result != 0; byte high byte preserved; memory autoincrement |
| TST / TSTB | 46 | 6 | 6 | N/Z from operand, V=0 C=0; no write; byte zero ignores high byte |
| ADC / ADCB | 52 | 8 | 4 | C=0 retains destination; C=1 adds one; C/V boundaries |
| SBC / SBCB | 53 | 10 | 10 | C=0 retains destination; C=1 subtracts one; result 0 clears C; borrow; V; N/Z |
| ASL / ASLB | 50 | 6 | 6 | C = old bit 15/7; V = N xor C; N/Z; byte high byte preserved |
| ASR / ASRB | 49 | 7 | 7 | C = old bit 0; sign replication; V = N xor C; N/Z; byte high byte preserved |
| ROL / ROLB | 55 | 6 | 6 | C = old bit 15/7; old C into bit 0/7; V = N xor C; N/Z |
| ROR / RORB | 56 | 6 | 6 | C = old bit 0; old C into bit 15/7; V = N xor C; N/Z |
| SWAB | 57 | 6 | 6 | Byte swap; N = result bit 7; Z = low byte of result zero; V=0 C=0; register and memory |
| Addressing | 22–35 | 10 | 10 | Byte/word step sizes; SP/PC always 2; deferred autoincrement/decrement always 2; index consumes a word and advances PC; side effects happen once; register-deferred does not modify its register |
| Operand helpers | 3.3, 3.4 | 6 | 6 | `resolve` applies side effects once; `read`/`write` are pure; byte/SP/index rules |
| Decode | 3.1, 3.2 | 8 | 8 | Single-operand opcode/mode/register/byte bit; SWAB; SUB is double word (not byte); MOVB is double byte; EIS is not double |

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

## Remaining failure: ADC/ADCB V and C

Handbook page 52:

```text
Operation: (dst) <- (dst) + (C)

N: set if result < 0; cleared otherwise
Z: set if result = 0; cleared otherwise
V: set if (dst) was 077777 and (C) was 1; cleared otherwise
C: set if (dst) was 177777 and (C) was 1; cleared otherwise
```

The byte forms use `0177` for V and `0377` for C. The four test cases are:

| Test | Old (dst) | C in | Result | V | C |
| --- | ---: | ---: | ---: | ---: | ---: |
| `adc_word_overflow` | `077777` | 1 | `100000` | 1 | 0 |
| `adc_word_carry_out` | `177777` | 1 | 0 | 0 | 1 |
| `adc_byte_overflow` | `0177` | 1 | `0200` | 1 | 0 |
| `adc_byte_carry_out` | `0377` | 1 | 0 | 0 | 1 |

Current `src/instructions/adc.cpp`:

```cpp
status.set_negative_zero(value, is_byte);
(oldvalue == 0177777) && status.get_carry()
  ? status.set_carry()
  : status.unset_carry();

if (is_byte) {
  ((oldvalue & 0xff) == 0177) && status.get_carry()
    ? status.set_overflow()
    : status.unset_overflow();
} else {
  (oldvalue == 0177777) && status.get_carry()
    ? status.set_overflow()
    : status.unset_overflow();
}
```

There are three errors:

1. **Word V uses `0177777` instead of `077777`.** This makes
   `adc_word_overflow` clear V (expected set), and makes
   `adc_word_carry_out` set V spuriously (expected clear).
2. **V reads `status.get_carry()` after C has already been updated.** It sees
   the outgoing carry, not the incoming carry. For `adc_byte_overflow`
   (`old = 0177`, C in = 1) the outgoing carry is 0, so V is cleared.
3. **Byte C compares the whole word with `0177777`.** For `adc_byte_carry_out`
   (`old = 0377`, C in = 1) the comparison is false, so C is not set.

### Suggested minimal fix

Capture the incoming carry before changing C, use the operand-size mask, and
test the old destination value:

```cpp
static void
set_status(StatusRegister status,
           uint16_t value,
           uint16_t oldvalue,
           bool is_byte)
{
  const bool carry_in = status.get_carry();
  const uint16_t mask = is_byte ? 0x00ff : 0xffff;
  const uint16_t sign = is_byte ? 0x0080 : 0x8000;

  status.set_negative_zero(value, is_byte);

  (carry_in && ((oldvalue & mask) == mask))
    ? status.set_carry()
    : status.unset_carry();

  (carry_in && ((oldvalue & mask) == static_cast<uint16_t>(sign - 1)))
    ? status.set_overflow()
    : status.unset_overflow();
}
```

`sign - 1` is `077777` for word and `0177` for byte. The rest of `adc()`
(including the new C=0 retention) is correct.

## Resolved in this round

These previously failing groups now pass:

- `write_operand` register fallthrough (3 tests)
- NEG/NEGB carry inversion (8 tests)
- SBC/SBCB with C=0 retention (2 tests)
- SWAB low-byte Z (1 test)
- ADC/ADCB with C=0 retention (2 tests)

## Notes on the review corrections

- **ADC/SBC with C=0:** retained destination is correct and now implemented;
  those tests pass.
- **SWAB N:** bit 7 of the result is correct; `swab_negative_from_low_byte`
  passes.
- **SBC V/C:** the C=1 cases all pass, so no change is needed there.
- **ADC flags:** the tests are aligned with the handbook's old-value
  conditions; the remaining mismatch is in `adc.cpp`, not in the tests.
