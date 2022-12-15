#pragma once

#include <unordered_map>
#include <vector>

#include "inode.h"

typedef std::unordered_map<std::string, std::unique_ptr<Inode>> DirMap;

class Dir: public Inode {
  public:
    Dir(std::string name, Inode* par = nullptr): Inode(name, par) {}

    void add_file(std::string name, int sz);
    void add_dir(std::string name, std::vector<Dir*> &dirlist);
    int size() override;
    Inode* get(std::string name);

  private:
    DirMap m_map;
};
