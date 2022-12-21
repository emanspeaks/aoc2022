#include <iostream>
#include <fstream>

#include "main.h"
#include "factory.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day19A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  Factory f;

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

  std::cout << f.sumMaxQuality() << "\n";

  std::cout << "Day19B\n";

  std::cout << f.firstThreeMaxGeodeProduct() << "\n";

  return 0;
}
