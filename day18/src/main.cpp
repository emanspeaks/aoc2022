#include <iostream>
#include <fstream>

#include "main.h"
#include "cube.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day18A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  CubeContainer cc;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        cc.parseAndInsert(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << cc.countFaces() << "\n";

  std::cout << "Day18B\n";

  std::cout << cc.countExteriorFaces() <<"\n";

  return 0;
}
