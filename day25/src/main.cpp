#include <iostream>
#include <fstream>

#include "main.h"
#include "snafu.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day25A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  SnafuConverter sc;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        sc.parseLine(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  std::cout << sc.snafuTotal() << "\n";

  std::cout << "Day25B\n";

  std::cout << 0 << "\n";

  return 0;
}
