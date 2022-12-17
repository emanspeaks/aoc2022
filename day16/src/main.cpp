#include <iostream>
#include <fstream>

#include "main.h"
#include "valves.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day16A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  Network net;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        net.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  // std::cout << net.dijkstraMaxCost() << "\n";
  // std::cout << net.randyMaxCost() << "\n";
  std::cout << net.dynProgMaxCost() << "\n";

  std::cout << "Day16B\n";
  std::cout << "\n";

  return 0;
}
