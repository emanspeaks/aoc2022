#pragma once

#include <string>
#include <vector>
#include <algorithm>

enum Facing {faceright, facedown, faceleft, faceup};

class ForceField {
  public:
    ForceField(): m_maxlen(0) {}

    void parseLine(const std::string &s, bool footer) {
      if (footer) m_instr = s;
      else {
        m_mapdata.push_back(s);
        m_maxlen = std::max(m_maxlen, (int)s.length());
      }
    }

    long long password() {
      Facing dir = faceright;
      int x = getRowStart(0), y = 0;

      std::string buf;
      for (auto c: m_instr) {
        if (c > '9') {
          moveForward(std::stoi(buf), x, y, dir);
          buf = "";
          dir = Facing(((c == 'R')? dir + 1 : dir + 3) % 4);
        } else buf += c;
      }
      moveForward(std::stoi(buf), x, y, dir);

      return 1000LL*(long long)(y + 1) + 4LL*(long long)(x + 1) + (long long)dir;
    }

  protected:
    std::vector<std::string> m_mapdata;
    std::string m_instr;
    int m_maxlen;

    int getRowStart(int y) {
      char z;
      int x = -1;
      do z = m_mapdata[y][++x]; while (z == ' ');
      return x;
    }

  private:
    void moveForward(int i, int &x, int &y, Facing dir) {
      switch (dir) {
        case faceleft: moveLeft(i, x, y); break;
        case faceright: moveRight(i, x, y); break;
        case faceup: moveUp(i, x, y); break;
        case facedown: moveDown(i, x, y); break;
      }
    }

    void moveLeft(int i, int &x, int y) {
      std::string &row = m_mapdata[y];
      int tmp;
      char z;
      while (i) {
        if (!x || row[x - 1] == ' ') {
          // look at the wraparound
          tmp = row.length();
          do z = row[--tmp]; while (z == ' ');
          if (z == '#') return; else x = tmp;
        } else if (row[x - 1] == '#') return;
        else x--;
        i--;
      }
    }

    void moveRight(int i, int &x, int y) {
      std::string &row = m_mapdata[y];
      int tmp;
      char z;
      while (i) {
        if (x == row.length() - 1 || row[x + 1] == ' ') {
          // look at the wraparound
          tmp = -1;
          do z = row[++tmp]; while (z == ' ');
          if (z == '#') return; else x = tmp;
        } else if (row[x + 1] == '#') return;
        else x++;
        i--;
      }
    }

    void moveUp(int i, int x, int &y) {
      int tmp;
      char z;
      while (i) {
        if (!y || x >= m_mapdata[y - 1].length() || m_mapdata[y - 1][x] == ' ') {
          // look at the wraparound
          tmp = m_mapdata.size();
          do z = (x < m_mapdata[--tmp].length())? m_mapdata[tmp][x] : ' '; while (z == ' ');
          if (z == '#') return; else y = tmp;
        } else if (m_mapdata[y - 1][x] == '#') return;
        else y--;
        i--;
      }
    }

    void moveDown(int i, int x, int &y) {
      int tmp;
      char z;
      while (i) {
        if (y == m_mapdata.size() - 1 || x >= m_mapdata[y + 1].length() || m_mapdata[y + 1][x] == ' ') {
          // look at the wraparound
          tmp = -1;
          do z = (x < m_mapdata[++tmp].length())? m_mapdata[tmp][x] : ' '; while (z == ' ');
          if (z == '#') return; else y = tmp;
        } else if (m_mapdata[y + 1][x] == '#') return;
        else y++;
        i--;
      }
    }
};
