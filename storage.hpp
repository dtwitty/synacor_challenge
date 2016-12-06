#ifndef STORAGE_H
#define STORAGE_H

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

const size_t MEMORY_SIZE = 32768;
const size_t NUM_REGISTERS = 8;

class Memory {
 public:
  Memory() { memory_.reserve(MEMORY_SIZE); }

  uint16_t& operator[](size_t i) {
    assert(i < MEMORY_SIZE);
    return memory_[i];
  }

  void Set(size_t i, uint16_t n) {
    assert(i < MEMORY_SIZE);
    memory_[i] = n;
  }

  uint16_t Get(size_t i) const {
    assert(i < MEMORY_SIZE);
    return memory_[i];
  }

 private:
  vector<uint16_t> memory_;
};

class Stack {
 public:
  Stack() {}

  void Push(uint16_t n) { stack_.push_back(n); }

  uint16_t Pop() {
    assert(!stack_.empty());
    uint16_t ret = stack_.back();
    stack_.pop_back();
    return ret;
  }

  bool Empty() { return stack_.empty(); }

 private:
  vector<uint16_t> stack_;
};

class Registers {
 public:
  Registers() { fill_n(register_file_, NUM_REGISTERS, 0); }

  uint16_t& operator[](size_t i) {
    assert(i < NUM_REGISTERS);
    return register_file_[i];
  }

  void Set(size_t i, uint16_t n) {
    assert(i < NUM_REGISTERS);
    register_file_[i] = n;
  }

  uint16_t Get(size_t i) const {
    assert(i < NUM_REGISTERS);
    return register_file_[i];
  }

 private:
  uint16_t register_file_[NUM_REGISTERS];
};

#endif  // STORAGE_H
