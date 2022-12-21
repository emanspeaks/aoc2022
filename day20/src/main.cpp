#include <iostream>
#include <fstream>

#include "main.h"
#include "ring.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day20A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  Ring r;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        r.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << r.coordSum() << "\n";

  std::cout << "Day20B\n";

  std::cout << r.coordSum2() << "\n";

  return 0;
}
