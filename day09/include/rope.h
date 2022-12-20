#pragma once

#include <string>
#include <deque>
#include <memory>
#include <array>
#include <iostream>
#include <queue>

#define ROPELEN 10

typedef std::deque<char> YData;
typedef YData* XPtr;
typedef std::deque<std::unique_ptr<YData>> XData;
typedef std::array<int, 2> Coord;
typedef std::array<char, 2> Instr;

const Coord hstart = {0, 0};

enum Dir {up, dn, lt, rt};

class Field {
  public:
    Field(): m_mx(0), m_my(0), m_px(-1), m_py(-1) {
      checkExtents(hstart);
    }

    void parseLine(const std::string &s) {
      char d;
      switch (s[0])
      {
        case 'U': {d = up; break;}
        case 'D': {d = dn; break;}
        case 'L': {d = lt; break;}
        case 'R': {d = rt; break;}
      }
      Instr tmp = {d, static_cast<char>(std::stoi(s.substr(2, s.length() - 2)))};
      m_instr.push(tmp);
      m_instr2.push(tmp);
    }

    int simpleRope() {
      int hx = hstart[0], hy = hstart[1], tx = hx, ty = hy;
      int dx, dy;
      Instr tmp;
      set(hstart, 1);
      while (m_instr.size()) {
        tmp = m_instr.front();
        m_instr.pop();
        dir2dxy(tmp[0], dx, dy);
        for (int i = 0; i < tmp[1]; i++) {
          hx += dx;
          hy += dy;
          updateTail(hx, hy, tx, ty);
          checkExtents({tx, ty});
          set(tx, ty, 1);
        }
      }

      int sum = 0;
      for (int y = m_my; y <= m_py; y++) for (int x = m_mx; x <= m_px; x++) sum += get(x, y);
      return sum;
    }

    int longRope() {
      int x[ROPELEN] = {hstart[0]}, y[10] = {hstart[1]};
      int dx, dy;
      Instr tmp;
      resetGrid();
      set(hstart, 1);
      while (m_instr2.size()) {
        tmp = m_instr2.front();
        m_instr2.pop();
        dir2dxy(tmp[0], dx, dy);
        for (int i = 0; i < tmp[1]; i++) {
          x[0] += dx;
          y[0] += dy;
          for (int j = 0; j < ROPELEN - 1; j++) {
            updateTail(x[j], y[j], x[j + 1], y[j + 1]);
          }
          checkExtents({x[ROPELEN - 1], y[ROPELEN - 1]});
          set(x[ROPELEN - 1], y[ROPELEN - 1], 1);
        }
      }

      int sum = 0;
      for (int y2 = m_my; y2 <= m_py; y2++) for (int x2 = m_mx; x2 <= m_px; x2++) sum += get(x2, y2);
      return sum;
    }

    void printGrid() {
      char v;
      for (int y = m_my; y <= m_py; y++) {
        for (int x = m_mx; x <= m_px; x++) {
          v = get(x, y);
          if (!v && x == hstart[0] && y == hstart[1]) std::cout << '+';
          else std::cout << std::to_string(v);
        }
        std::cout << "\n";
      }
    }

  private:
    std::queue<Instr> m_instr, m_instr2;

    XData m_grid;
    int m_mx, m_my, m_px, m_py;

    void addPX() {m_grid.push_back(std::make_unique<YData>());}
    void addMX() {m_grid.push_front(std::make_unique<YData>());}

    void addPY() {for (auto &x: m_grid) addPY(x.get());}
    void addPY(XPtr x) {
      x->push_back(0);
    }

    void addMY() {for (auto &x: m_grid) addMY(x.get());}
    void addMY(XPtr x) {
      x->push_front(0);
    }

    void addAndFillPX() {
      addPX();
      XPtr pxp = m_grid.back().get();
      for (int i = m_py - m_my + 1; i > 0; i--) addPY(pxp);
    }

    void addAndFillMX() {
      addMX();
      XPtr mxp = m_grid.front().get();
      for (int i = m_py - m_my + 1; i > 0; i--) addPY(mxp);
    }

    void set(const Coord c, char v) {
      set(c[0], c[1], v);
    }

    void set(int x, int y, char v) {
      m_grid[x - m_mx].get()->at(y - m_my) = v;
    }

    char get(const Coord c) {
      return get(c[0], c[1]);
    }

    char get(int x, int y) {
      return m_grid[x - m_mx].get()->at(y - m_my);
    }

    void checkExtents(const Coord c) {
      const int x = c[0], y = c[1];

      if (m_mx > m_px) {
        addPX();
        m_mx = m_px = x;
      }
      if (m_my > m_py) {
        addPY();
        m_my = m_py = y;
      }

      if (x > m_px) {
        for (int i = x - m_px; i > 0; i--) addAndFillPX();
        m_px = x;
      } else if (x < m_mx) {
        for (int i = m_mx - x; i > 0; i--) addAndFillMX();
        m_mx = x;
      }

      if (y > m_py) {
        for (int i = y - m_py; i > 0; i--) addPY();
        m_py = y;
      } else if (y < m_my) {
        for (int i = m_my - y; i > 0; i--) addMY();
        m_my = y;
      }
    }

    void updateTail(int hx, int hy, int &tx, int &ty) {
      int dx = hx - tx, dy = hy - ty;

      // If the head is ever two steps directly up, down, left, or right from the tail,
      // the tail must also move one step in that direction so it remains close enough
      if (dx == 2 && !dy) tx++;
      else if (dx == -2 && !dy) tx--;
      else if (dy == 2 && !dx) ty++;
      else if (dy == -2 && !dx) ty--;

      // Otherwise, if the head and tail aren't touching and aren't in the same row or column,
      // the tail always moves one step diagonally to keep up
      // .....    .....    .....
      // .....    ..H..    ..H..
      // ..H.. -> ..... -> ..T..
      // .T...    .T...    .....
      // .....    .....    .....

      // .....    .....    .....
      // .....    .....    .....
      // ..H.. -> ...H. -> ..TH.
      // .T...    .T...    .....
      // .....    .....    .....
      else if ((dx == 1 && dy == 2) || (dx == 2 && dy == 1) || (dx == 2 && dy == 2)) {tx++; ty++;}
      else if ((dx == -1 && dy == 2) || (dx == -2 && dy == 1) || (dx == -2 && dy == 2)) {tx--; ty++;}
      else if ((dx == 1 && dy == -2) || (dx == 2 && dy == -1) || (dx == 2 && dy == -2)) {tx++; ty--;}
      else if ((dx == -1 && dy == -2) || (dx == -2 && dy == -1) || (dx == -2 && dy == -2)) {tx--; ty--;}
    }

    void dir2dxy(char d, int &dx, int&dy) {
      switch (d)
      {
        case up: {dx = 0; dy = 1; break;}
        case dn: {dx = 0; dy = -1; break;}
        case lt: {dx = -1; dy = 0; break;}
        case rt: {dx = 1; dy = 0; break;}
      }
    }

    void resetGrid() {
      m_grid.clear();
      m_mx = 0;
      m_my = 0;
      m_px = -1;
      m_py = -1;
      checkExtents(hstart);
    }
};
