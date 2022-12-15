#include <iostream>
#include <fstream>
#include <sstream>

#include "main.h"

int main(int argc, char const *argv[]) {
  std::cout << "Day05A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  bool body = false;
  std::stack<char> stk[STACKCOUNT], stkb[STACKCOUNT];
  std::vector<char> tmp[STACKCOUNT];

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        if (body) {
          parse_instruction(line, stk);
          parse_instruction(line, stkb, true);
        } else {
          parse_stack_line(line, tmp);
        }
      } else {
        if (!body) {
          build_stacks(stk, tmp);
          build_stacks(stkb, tmp);
        }
        body = true;
      }
    }
  } else {
    std::cout << "failed to open input.txt";
  }

  for (int i = 0; i < STACKCOUNT; i++) std::cout << stk[i].top();
  std::cout << "\n";

  std::cout << "Day05B\n";
  for (int i = 0; i < STACKCOUNT; i++) std::cout << stkb[i].top();
  std::cout << "\n";

  return 0;
}

void parse_instruction(const std::string &s, std::stack<char> (&stk)[STACKCOUNT], bool multi) {
  std::stringstream sstm(s);
  std::vector<std::string> svec;
  std::string tmp;
  std::vector<char> x;
  while (std::getline(sstm, tmp, ' ')) svec.push_back(tmp);
  int from = std::stoi(svec[3]) - 1, to = std::stoi(svec[5]) - 1;
  for (int n = std::stoi(svec[1]); n > 0; n--) {
    if (multi) x.push_back(stk[from].top()); else stk[to].push(stk[from].top());
    stk[from].pop();
  }
  if (multi) for (std::vector<char>::reverse_iterator z = x.rbegin(); z != x.rend(); z++)
    stk[to].push(*z);
}

void parse_stack_line(const std::string &s, std::vector<char> (&vec)[STACKCOUNT]) {
  for (int i = 0; i < STACKCOUNT; i++) if (s[i*4] == '[') vec[i].push_back(s[i*4 + 1]);
}

void build_stacks(std::stack<char> (&stk)[STACKCOUNT], std::vector<char> (&vec)[STACKCOUNT]) {
  for (int i = 0; i < STACKCOUNT; i++)
    for (std::vector<char>::reverse_iterator j = vec[i].rbegin(); j != vec[i].rend(); j++)
      stk[i].push(*j);
}
