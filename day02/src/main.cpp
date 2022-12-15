#include <iostream>
#include <fstream>

#include "main.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day02A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  int sum = 0;
  int sumb = 0;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        sum += score_round(line);
        sumb += score_round_b(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
  }

  std::cout << sum << "\n";

  std::cout << "Day02B\n";
  std::cout << sumb << "\n";

  return 0;
}

int score_round(const std::string &s) {
  // rock = 0, paper = 1, scissors = 2
  char theirs = s[0] - 'A';
  char mine = s[2] - 'X';
  int outcome = 0;
  if (mine == theirs) outcome = 3;
  else if (mine == ((theirs + 1)%3)) outcome = 6;
  return outcome + mine + 1;
}

int score_round_b(const std::string &s) {
  // rock = 0, paper = 1, scissors = 2
  // lose = 0, draw = 1, win = 2
  char theirs = s[0] - 'A';
  char outcome_raw = s[2] - 'X';
  int outcome = outcome_raw*3;
  char mine = (theirs + ((outcome_raw + 2)%3))%3;
  return outcome + mine + 1;
}
