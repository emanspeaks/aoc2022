#include <iostream>
#include <fstream>

#include "main.h"
#include "monkeys.h"

#define ROUNDCOUNT 20
#define ROUNDCOUNTPT2 10000

int main(int argc, char const *argv[]) {
  std::cout << "Day11A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  MonkeySim ms, ms2;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        ms.parseLine(line);
        ms2.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << ms.runSim(ROUNDCOUNT) << "\n";

  std::cout << "Day11B\n";

  std::cout << ms2.runSim(ROUNDCOUNTPT2, true) <<"\n";
  // std::cout << ms2.runSim(ROUNDCOUNTPT2, true) <<"\n";

  return 0;
}
