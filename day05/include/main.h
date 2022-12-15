#pragma once

#include <string>
#include <stack>
#include <vector>

#define STACKCOUNT 9

int main(int argc, char const *argv[]);
void parse_instruction(const std::string &s, std::stack<char> (&stk)[STACKCOUNT], bool multi = false);
void parse_stack_line(const std::string &s, std::vector<char> (&vec)[STACKCOUNT]);
void build_stacks(std::stack<char> (&stk)[STACKCOUNT], std::vector<char> (&vec)[STACKCOUNT]);
