#include <iostream>
#include <fstream>

#include "main.h"

#define SOPLEN 4
#define SOMLEN 14

int main(int argc, char const *argv[]) {
  std::cout << "Day06A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  int sopend, somend;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        sopend = sop_marker_end(line);
        somend = som_marker_end(line);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
  }

  std::cout << sopend << "\n";

  std::cout << "Day06B\n";
  std::cout << somend << "\n";

  return 0;
}

int sop_marker_end(const std::string &s) {
  int len = s.length() - SOPLEN + 1;
  std::string tmp;
  bool test;
  for (int i = 0; i < len; i++) {
    test = true;
    for (int j = i; j < (i + SOPLEN - 1); j++)
      for (int k = j + 1; k < (i + SOPLEN); k++)
        if (s[j] == s[k]) test = false;
    if (test) return i + SOPLEN;
  }
  return -1;
}

int som_marker_end(const std::string &s) {
  int len = s.length() - SOMLEN + 1;
  std::string tmp;
  bool test;
  for (int i = 0; i < len; i++) {
    test = true;
    for (int j = i; j < (i + SOMLEN - 1); j++)
      for (int k = j + 1; k < (i + SOMLEN); k++)
        if (s[j] == s[k]) test = false;
    if (test) return i + SOMLEN;
  }
  return -1;
}
