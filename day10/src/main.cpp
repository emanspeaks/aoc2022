#include <iostream>
#include <fstream>
#include <queue>

#include "main.h"

#define OFFSET 20
#define INTERVAL 40
#define PT1CYCLES 220
#define PT2CYCLES (40*6)

int main(int argc, char const *argv[]) {
  std::cout << "Day10A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  std::queue<int> instr;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        if (line[0] == 'n') instr.push(0);
        else instr.push(std::stoi(line.substr(5, line.length()-5)));
      }
    }
  } else {
    std::cout << "failed to open input.txt";
    return 1;
  }

  // for (;instr.size();instr.pop()) std::cout << instr.front() << "\n";

  std::queue<int> instr2(instr);

  int x = 1, cur = 0, sum = 0;
  bool wait = false;

  for (int i = 0; i < 1 + PT1CYCLES; i++) {
    if (!((i - OFFSET) % INTERVAL)) {
      sum += i*x;
    }
    if (!wait && cur) x += cur;
    if (wait) {wait = false; continue;}
    cur = instr.front();
    instr.pop();
    if (cur) {
      wait = true;
    }
  }

  std::cout << sum << "\n";

  std::cout << "Day10B\n";

  x = 1;
  cur = 0;
  wait = false;
  for (int i = 0; i < 1 + PT2CYCLES; i++) {
    if (!(i % INTERVAL)) std::cout << "\n";
    if (!wait && cur) x += cur;
    if (i < PT2CYCLES) {
      if (i % INTERVAL >= x - 1 && i % INTERVAL <= x + 1) std::cout << '#';
      else std::cout << '.';
    } else break;
    if (wait) {wait = false; continue;}
    cur = instr2.front();
    instr2.pop();
    if (cur) {
      wait = true;
    }
  }

  std::cout <<"\n";

  return 0;
}
