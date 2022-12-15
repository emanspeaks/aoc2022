#pragma once

#include <string>
#include <unordered_set>

int main(int argc, char const *argv[]);
int parse_rucksack(const std::string &s);
char priority(const char c);
int parse_trio(const std::string trio[3]);
std::unordered_set<char> unordset_intersect(const std::unordered_set<char> &s1,
  const std::unordered_set<char> &s2);
