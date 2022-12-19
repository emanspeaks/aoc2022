#include <iostream>
#include <fstream>
#include <algorithm>

#include "main.h"
#include "sand.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day14A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  Field f;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        f.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  int sandcount = f.maxSand();
  std::cout << sandcount << "\n";

  std::cout << "Day14B\n";

  f.addFloor();
  std::cout << f.maxSand(true, sandcount) <<"\n";
  // f.printGrid();

  return 0;
}
