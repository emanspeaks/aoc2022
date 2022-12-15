#include <iostream>
#include <fstream>

#include "main.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day04A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  int x[4], sum = 0, sumb = 0;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        parse_pair(line, x);
        sum += complete_overlap(x);
        sumb += partial_overlap(x);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
  }

  std::cout << sum << "\n";

  std::cout << "Day04B\n";
  std::cout << sumb << "\n";

  return 0;
}

void parse_pair(const std::string &s, int (&x)[4]) {
  int i = 0, j = 0, s0 = 0, len = s.length();
  for (std::string::const_iterator c = s.begin(); c != s.end(); c++, j++) {
    if (*c == '-' || *c == ',') {
      x[i++] = std::stoi(s.substr(s0, j - s0));
      s0 = j + 1;
    }
  }
  x[i] = std::stoi(s.substr(s0, len - s0));
}

int partial_overlap(const int (&x)[4]) {
  return (x[0] <= x[2] && x[1] >= x[2]) || (x[2] <= x[0] && x[3] >= x[0]);
}

int complete_overlap(const int (&x)[4]) {
  return (x[0] <= x[2] && x[1] >= x[3]) || (x[2] <= x[0] && x[3] >= x[1]);
}
