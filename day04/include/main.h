#pragma once

#include <string>

int main(int argc, char const *argv[]);
void parse_pair(const std::string &s, int (&x)[4]);
int partial_overlap(const int (&x)[4]);
int complete_overlap(const int (&x)[4]);
