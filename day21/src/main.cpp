#include <iostream>
#include <fstream>

#include "main.h"
#include "monkcalc.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day21A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  MonkCalc mc;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        mc.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << mc.root() << "\n";

  std::cout << "Day21B\n";

  std::cout << mc.humn() << "\n";

  return 0;
}
