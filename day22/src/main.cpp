#include <iostream>
#include <fstream>

#include "main.h"
#include "forcefield.h"
#include "uvcube.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day22A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  ForceField ff;
  bool footer = false;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        ff.parseLine(line, footer);
      } else footer = true;
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << ff.password() << "\n";

  std::cout << "Day22B\n";

  UVCube uvc(ff);

  std::cout << uvc.password() << "\n";

  return 0;
}
