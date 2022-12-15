#include "dir.h"
#include "aocfile.h"

void Dir::add_file(std::string name, int sz) {
  m_map[name] = std::make_unique<AocFile>(name, this, sz);
}

void Dir::add_dir(std::string name, std::vector<Dir*> &dirlist) {
  m_map[name] = std::make_unique<Dir>(name, this);
  dirlist.push_back(static_cast<Dir*>(m_map[name].get()));
}

int Dir::size() {
  int sz = 0;
  for (auto &x: m_map) sz += x.second.get()->size();
  return sz;
}

Inode* Dir::get(std::string name) {
  return m_map[name].get();
}
