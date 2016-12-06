#ifndef VM_H
#define VM_H

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

#include <storage.hpp>

using namespace std;

const size_t NUM_OPCODES = 22;
const uint16_t MAX_LITERAL = 32767;
const uint16_t MIN_REGISTER = 32768;
const uint16_t MAX_REGISTER = 32775;

enum OpCode {
  HALT = 0,
  SET = 1,
  PUSH = 2,
  POP = 3,
  EQ = 4,
  GT = 5,
  JMP = 6,
  JT = 7,
  JF = 8,
  ADD = 9,
  MULT = 10,
  MOD = 11,
  AND = 12,
  OR = 13,
  NOT = 14,
  RMEM = 15,
  WMEM = 16,
  CALL = 17,
  RET = 18,
  OUT = 19,
  IN = 20,
  NOOP = 21,
};

class SynacorVM {
 public:
  SynacorVM() { program_counter_ = 0; }

  void Load(const string& file_name) {
    unsigned char bytes[2];
    FILE* fp = fopen(file_name.c_str(), "rb");

    uint16_t ptr = 0;

    while (fread(bytes, 2, 1, fp)) {
      uint16_t n = static_cast<uint16_t>(bytes[0]) |
                   (static_cast<uint16_t>(bytes[1]) << 8);
      memory_.Set(ptr, n);
      ptr += 1;
    }
  }

  void Loop() {
    uint16_t next_program_counter = 0;
    while (RunOneInstruction(&next_program_counter)) {
      program_counter_ = next_program_counter;
    }
  }

  bool RunOneInstruction(uint16_t* next_program_counter) {
    OpCode opcode = ReadOpCode();
    switch (opcode) {
      case HALT: {
        return false;
      }
      case SET: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a = ValueOfRegisterOrLiteral(b);

        *next_program_counter = program_counter_ + 3;
        return true;
      }
      case PUSH: {
        uint16_t a = ReadAddressAtOffset(1);
        stack_.Push(ValueOfRegisterOrLiteral(a));

        *next_program_counter = program_counter_ + 2;
        return true;
      }
      case POP: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a = stack_.Pop();

        *next_program_counter = program_counter_ + 2;
        return true;
      }
      case EQ: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);
        uint16_t c = ReadAddressAtOffset(3);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a =
            ValueOfRegisterOrLiteral(b) == ValueOfRegisterOrLiteral(c);

        *next_program_counter = program_counter_ + 4;
        return true;
      }
      case GT: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);
        uint16_t c = ReadAddressAtOffset(3);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a =
            ValueOfRegisterOrLiteral(b) > ValueOfRegisterOrLiteral(c);

        *next_program_counter = program_counter_ + 4;
        return true;
      }
      case JMP: {
        uint16_t a = ReadAddressAtOffset(1);

        *next_program_counter = ValueOfRegisterOrLiteral(a);
        return true;
      }
      case JT: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);

        if (ValueOfRegisterOrLiteral(a)) {
          *next_program_counter = ValueOfRegisterOrLiteral(b);
        } else {
          *next_program_counter = program_counter_ + 3;
        }
        return true;
      }
      case JF: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);

        if (ValueOfRegisterOrLiteral(a)) {
          *next_program_counter = program_counter_ + 3;
        } else {
          *next_program_counter = ValueOfRegisterOrLiteral(b);
        }
        return true;
      }
      case ADD: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);
        uint16_t c = ReadAddressAtOffset(3);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        uint64_t n = static_cast<uint64_t>(ValueOfRegisterOrLiteral(b)) +
                     static_cast<uint64_t>(ValueOfRegisterOrLiteral(c));
        n %= 32768;
        reference_to_a = static_cast<uint16_t>(n);

        *next_program_counter = program_counter_ + 4;
        return true;
      }
      case MULT: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);
        uint16_t c = ReadAddressAtOffset(3);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        uint64_t n = static_cast<uint64_t>(ValueOfRegisterOrLiteral(b)) *
                     static_cast<uint64_t>(ValueOfRegisterOrLiteral(c));
        n %= 32768;
        reference_to_a = static_cast<uint16_t>(n);

        *next_program_counter = program_counter_ + 4;
        return true;
      }
      case MOD: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);
        uint16_t c = ReadAddressAtOffset(3);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        uint64_t n = static_cast<uint64_t>(ValueOfRegisterOrLiteral(b)) %
                     static_cast<uint64_t>(ValueOfRegisterOrLiteral(c));
        reference_to_a = static_cast<uint16_t>(n);

        *next_program_counter = program_counter_ + 4;
        return true;
      }
      case AND: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);
        uint16_t c = ReadAddressAtOffset(3);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a =
            ValueOfRegisterOrLiteral(b) & ValueOfRegisterOrLiteral(c);

        *next_program_counter = program_counter_ + 4;
        return true;
      }
      case OR: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);
        uint16_t c = ReadAddressAtOffset(3);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a =
            ValueOfRegisterOrLiteral(b) | ValueOfRegisterOrLiteral(c);

        *next_program_counter = program_counter_ + 4;
        return true;
      }
      case NOT: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a = (~ValueOfRegisterOrLiteral(b)) & ((1 << 15) - 1);

        *next_program_counter = program_counter_ + 3;
        return true;
      }
      case RMEM: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a = memory_.Get(ValueOfRegisterOrLiteral(b));

        *next_program_counter = program_counter_ + 3;
        return true;
      }
      case WMEM: {
        uint16_t a = ReadAddressAtOffset(1);
        uint16_t b = ReadAddressAtOffset(2);

        memory_.Set(ValueOfRegisterOrLiteral(a), ValueOfRegisterOrLiteral(b));

        *next_program_counter = program_counter_ + 3;
        return true;
      }
      case CALL: {
        uint16_t a = ReadAddressAtOffset(1);

        stack_.Push(program_counter_ + 2);
        *next_program_counter = ValueOfRegisterOrLiteral(a);
        return true;
      }
      case RET: {
        if (stack_.Empty()) {
          return false;
        }
        uint16_t a = stack_.Pop();

        *next_program_counter = a;
        return true;
      }
      case OUT: {
        uint16_t a = ReadAddressAtOffset(1);

        putchar(ValueOfRegisterOrLiteral(a));

        *next_program_counter = program_counter_ + 2;
        return true;
      }
      case IN: {
        uint16_t a = ReadAddressAtOffset(1);

        uint16_t& reference_to_a = ReferenceToRegister(a);
        reference_to_a = getchar();

        *next_program_counter = program_counter_ + 2;
        return true;
      }
      case NOOP: {
        *next_program_counter = program_counter_ + 1;
        return true;
      }
      default: { return false; }
    }
  }

 private:
  bool IsLiteral(uint16_t n) { return n <= MAX_LITERAL; }

  bool IsRegister(uint16_t n) {
    return (n >= MIN_REGISTER) && (n <= MAX_REGISTER);
  }

  uint16_t& ReferenceToRegister(uint16_t n) {
    assert(IsRegister(n));
    return registers_[n - MIN_REGISTER];
  }

  uint16_t ValueOfRegisterOrLiteral(uint16_t n) {
    assert(IsLiteral(n) || IsRegister(n));
    if (IsLiteral(n)) {
      return n;
    } else {
      return ReferenceToRegister(n);
    }
  }

  uint16_t ReadAddressAtOffset(uint16_t offset) {
    return memory_.Get(program_counter_ + offset);
  }

  OpCode ReadOpCode() {
    uint16_t opcode_int = memory_[program_counter_];
    assert(opcode_int < NUM_OPCODES);
    return static_cast<OpCode>(opcode_int);
  }

  // Storage Regions
  Memory memory_;
  Stack stack_;
  Registers registers_;

  // Execution
  size_t program_counter_;
};

#endif  // VM_H
