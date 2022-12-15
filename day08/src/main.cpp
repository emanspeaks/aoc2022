#include <iostream>
#include <fstream>

#include "main.h"
#include "vizmap.h"
#include "scnmap.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day08A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  Forest trees;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        trees.push_back(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  VizMap vm(trees);
  std::cout << vm.sum() << "\n";

  std::cout << "Day08B\n";

  ScenicMap sm(trees);
  std::cout << sm.max() << "\n";

  return 0;
}
