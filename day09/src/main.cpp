#include <iostream>
#include <fstream>

#include "main.h"
#include "rope.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day09A\n";

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

  std::cout << f.simpleRope() << "\n";

  std::cout << "Day09B\n";

  std::cout << f.longRope() << "\n";

  return 0;
}
