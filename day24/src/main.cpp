#include <iostream>
#include <fstream>

#include "main.h"
#include "blizzard.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day24A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  BlizzardValley bv;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        bv.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << bv.shortestDuration() << "\n";

  std::cout << "Day24B\n";

  std::cout << bv.shortestDurDoubleBack() << "\n";

  return 0;
}
