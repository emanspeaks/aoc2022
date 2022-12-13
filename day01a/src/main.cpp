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
  std::cout << elves.back();

	return 0;
}
