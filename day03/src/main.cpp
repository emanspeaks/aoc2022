#include <iostream>
#include <fstream>
#include <unordered_set>

#include "main.h"

#define OFFSET ('a' - 'A' + 26)

int main(int argc, char const *argv[]) {
  std::cout << "Day03A\n";

  std::ifstream ifs("input.txt");
  std::string line, trio[3];
  int sum = 0, sumb = 0, i = 0;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        sum += parse_rucksack(line);
        trio[i++] = line;
        if (i == 3) {
          sumb += parse_trio(trio);
          i = 0;
        }
      }
    }
  } else {
    std::cout << "failed to open input.txt";
  }

  std::cout << sum << "\n";

  std::cout << "Day03B\n";
  std::cout << sumb << "\n";

  return 0;
}

int parse_rucksack(const std::string &s) {
  int len = s.length(), halflen = len/2, sum = 0;
  std::unordered_set<char> dups;

  for (int i = 0; i < halflen; i++) {
    for (int j = halflen; j < len; j++) {
      if (s[i] == s[j]) dups.insert(s[i]);
    }
  }
  for (std::unordered_set<char>::iterator k = dups.begin(); k != dups.end(); k++)
    sum += priority(*k);
  return sum;
}

char priority(const char c) {
  char a = c - 'a';
  if (a < 0) a += OFFSET;
  return a + 1;
}

int parse_trio(const std::string trio[3]) {
  int sum = 0;
  std::unordered_set<char> letters[3], tmp, dups;

  for (int i = 0; i < 3; i++)
    for (std::string::const_iterator c = trio[i].begin(); c != trio[i].end(); c++)
      letters[i].insert(*c);

  tmp = unordset_intersect(letters[0], letters[1]);
  dups = unordset_intersect(letters[2], tmp);

  for (std::unordered_set<char>::iterator k = dups.begin(); k != dups.end(); k++)
    sum += priority(*k);
  return sum;
}

std::unordered_set<char> unordset_intersect(const std::unordered_set<char> &s1,
  const std::unordered_set<char> &s2)
{
  std::unordered_set<char> out;

  for (std::unordered_set<char>::iterator k = s1.begin(); k != s1.end(); k++)
    if (s2.find(*k) != s2.end()) out.insert(*k);

  return out;
}
