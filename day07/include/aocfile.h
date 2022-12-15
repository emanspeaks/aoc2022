#pragma once

#include "inode.h"

class AocFile: public Inode {
  public:
    AocFile(std::string name, Inode* par, int sz): Inode(name, par), m_size(sz) {}

    int size() {return m_size;}

  private:
    int m_size;
};
