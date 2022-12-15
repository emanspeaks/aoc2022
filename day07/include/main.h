#pragma once

#include <string>
#include "dir.h"

int main(int argc, char const *argv[]);
void parseLine(const std::string &s, Dir* &cwd, Dir* root, std::vector<Dir*> &dirlist);
bool bySize(Dir* i, Dir* j);
