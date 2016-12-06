#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

#include <vm.hpp>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "need a binary file to load!" << endl;
  }

  SynacorVM vm;
  cout << "Loading file..." << endl;
  vm.Load(argv[1]);
  cout << "Loaded file." << endl;
  cout << "Executing..." << endl;
  vm.Loop();

  return 0;
}
