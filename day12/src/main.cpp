#include <iostream>
#include <fstream>

#include "main.h"
#include "topomap.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day12A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  TopoMap tm;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        tm.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << tm.minSteps() << "\n";

  std::cout << "Day12B\n";

  std::cout << tm.minStepsFromA() <<"\n";

  return 0;
}
