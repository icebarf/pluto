cmake_minimum_required(VERSION 3.20)
project(pdp11_single_operand_tests CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(TEST_SUPPORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/support)

# Tests are grouped by category. Glob only the test subdirectories so generated
# files under tests/build are never picked up.
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/decode/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/addressing/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/operand/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/instructions/*.cpp
)

# Compile the ISA implementation directly. src/main.cpp is intentionally not
# included: tests/main.cpp provides main() for this target.
set(INSTRUCTION_SOURCES
  ${SRC_DIR}/utilities.cpp
  ${SRC_DIR}/instructions/instructions.cpp
  ${SRC_DIR}/instructions/clr.cpp
  ${SRC_DIR}/instructions/com.cpp
  ${SRC_DIR}/instructions/inc.cpp
  ${SRC_DIR}/instructions/dec.cpp
  ${SRC_DIR}/instructions/neg.cpp
  ${SRC_DIR}/instructions/tst.cpp
  ${SRC_DIR}/instructions/asl.cpp
  ${SRC_DIR}/instructions/asr.cpp
  ${SRC_DIR}/instructions/rol.cpp
  ${SRC_DIR}/instructions/ror.cpp
  ${SRC_DIR}/instructions/adc.cpp
  ${SRC_DIR}/instructions/sbc.cpp
  ${SRC_DIR}/instructions/swab.cpp
)

add_executable(pdp11_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${TEST_SOURCES}
  ${INSTRUCTION_SOURCES}
)

target_include_directories(pdp11_tests PRIVATE
  ${SRC_DIR}
  ${SRC_DIR}/instructions
  ${TEST_SUPPORT_DIR}
)

enable_testing()
add_test(NAME pdp11_single_operand_tests COMMAND pdp11_tests)
