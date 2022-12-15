#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "main.h"

#define MAXDIRSIZE 100000
#define MAXDISKSIZE 70000000
#define SPACENEEDED 30000000

int main(int argc, char const *argv[]) {
  std::cout << "Day07A\n";

  std::ifstream ifs("input.txt");
  std::string line;
  Dir root("");
  Dir* cwd = &root;
  std::vector<Dir*> dirlist;

  if (ifs.is_open()){
    while (ifs) {
      std::getline(ifs, line);
      if (line.length()) {
        parseLine(line, cwd, &root, dirlist);
      }
    }
  } else {
    std::cout << "failed to open input.txt";
  }

  int tmp, rootsize = root.size(), sum = (rootsize < MAXDIRSIZE)? rootsize : 0;
  for (auto i: dirlist) {
    tmp = i->size();
    if (tmp < MAXDIRSIZE) sum += tmp;
  }
  std::cout << sum << "\n";

  std::cout << "Day07B\n";

  int freespace = MAXDISKSIZE - rootsize, deletetgt = SPACENEEDED - freespace;

  std::sort(dirlist.begin(), dirlist.end(), bySize);
  int j = 0;
  while (dirlist[j]->size() < deletetgt) j++;
  std::cout << dirlist[j]->size() << "\n";

  return 0;
}

void parseLine(const std::string &s, Dir* &cwd, Dir* root, std::vector<Dir*> &dirlist) {
  std::vector<std::string> stok;
  std::stringstream sstm(s);
  std::string tmp;
  while (std::getline(sstm, tmp, ' ')) stok.push_back(tmp);
  if (stok[0] == "$") {
    if (stok[1] == "cd") {
      if (stok[2] == "/") cwd = root;
      else if (stok[2] == "..") cwd = static_cast<Dir*>(cwd->parent());
      else cwd = (Dir*)cwd->get(stok[2]);
    }
  } else if (stok[0] == "dir") cwd->add_dir(stok[1], dirlist);
  else cwd->add_file(stok[1], std::stoi(stok[0]));
}

bool bySize(Dir* i, Dir* j) {
  return (i->size() < j->size());
}
