#pragma once

#include <string>
#include <memory>

class Inode {
  public:
    Inode(std::string name, Inode* par): m_name(name), m_parent(par) {}

    virtual int size() = 0;
    Inode* parent() {return m_parent;}
    std::string getName() {return m_name;}

  protected:
    std::string m_name;
    Inode* m_parent;
};
