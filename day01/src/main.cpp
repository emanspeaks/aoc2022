#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "main.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day01A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  std::vector<int> elves;
  int x = 0;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        x += std::stoi(line);
      } else {
        elves.push_back(x);
        x = 0;
      }
    }
  } else {
    std::cout << "failed to open input.txt";
  }

  std::sort(elves.begin(), elves.end());
  int sum = elves.rbegin()[0];
  std::cout << sum << "\n";

  std::cout << "Day01B\n";
  for (int i = 1; i < 3; i++) sum += elves.rbegin()[i];
  std::cout << sum << "\n";

  return 0;
}
