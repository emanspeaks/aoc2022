#include <iostream>
#include <fstream>

#include "main.h"
#include "elfsim.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day23A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  ElfSim es;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        es.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << es.emptySpaces() << "\n";

  std::cout << "Day23B\n";

  std::cout << es.totalRounds() << "\n";

  return 0;
}
