#pragma once

#include <vector>
#include <string>
#include <deque>
#include <memory>
#include <sstream>
#include <array>
#include <iostream>

typedef std::deque<char> YData;
typedef YData* XPtr;
typedef std::deque<std::unique_ptr<YData>> XData;
typedef std::array<int, 2> Coord;

const Coord emitter = {500, 0};

class Field {
  public:
    Field(): m_mx(0), m_my(0), m_px(-1), m_py(-1) {}

    void parseLine(const std::string &s) {
      std::stringstream sstm(s);
      std::string tmp;
      std::vector<std::string> stok;
      int i = 0;
      while (std::getline(sstm, tmp, ' ')) {
        if (i++ % 2) continue;
        stok.push_back(tmp);
      }

      i = 0;
      Coord c1, c2;
      for (auto coord: stok) {
        parseCoord(coord, c1);
        if (i++) {
          addLine(c1, c2);
        }
        c2 = c1;
      }
    }

    void addFloor() {
      // previous step added a y buffer already, only need to add one here.
      int floory = m_py + 1;
      // add three to make sure nothing tries to roll off the edge
      addLine({m_mx - 3, floory}, {m_px + 3, floory});
    }

    int maxSand(bool part2 = false, int n = 0) {
      checkExtents(emitter);

      // add buffers for final grain that might roll off the side
      if (!part2) {
        checkExtents({m_mx - 1, m_py + 1});
        checkExtents({m_px + 1, m_py});
      }

      // printGrid();

      int sandx = emitter[0], sandy = emitter[1], dx, dy;
      while (sandy < m_py) {
        dx = 0;
        dy = 1;
        // check next step
        if (get(sandx, sandy + 1)) {
          dx = -1;
          if (part2) checkExtents({sandx + dx, sandy + dy}, true);
          if (get(sandx - 1, sandy + 1)) {
            dx = 1;
            if (part2) checkExtents({sandx + dx, sandy + dy}, true);
            if (get(sandx + 1, sandy + 1)) {
              dx = 0;
              dy = 0;
            }
          }
        }
        // take next step, if possible
        // this grain is stopped, add to map and start a new grain
        if (dx == 0 && dy == 0) {
          set(sandx, sandy, 1);
          n++;
          if (sandx == emitter[0] && sandy == emitter[1]) break;
          sandx = emitter[0];
          sandy = emitter[1];
        } else {
          sandx += dx;
          sandy += dy;
        }
      }
      return n;
    }

    void printGrid() {
      char v;
      for (int y = m_my; y <= m_py; y++) {
        for (int x = m_mx; x <= m_px; x++) {
          v = get(x, y);
          if (!v && x == emitter[0] && y == emitter[1]) std::cout << '+';
          else std::cout << std::to_string(v);
        }
        std::cout << "\n";
      }
    }

  private:
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

    void checkExtents(const Coord c, bool part2 = false) {
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
        if (part2) for (int i = m_px; i <= x; i++) set(i, m_py, 1);
        m_px = x;
      } else if (x < m_mx) {
        for (int i = m_mx - x; i > 0; i--) addAndFillMX();
        int tmp = m_mx;
        m_mx = x;
        if (part2) for (int i = tmp; i >= x; i--) set(i, m_py, 1);
      }

      if (y > m_py) {
        for (int i = y - m_py; i > 0; i--) addPY();
        m_py = y;
      } else if (y < m_my) {
        for (int i = m_my - y; i > 0; i--) addMY();
        m_my = y;
      }
    }

    void parseCoord(const std::string &s, Coord &c) {
      std::stringstream sstm(s);
      std::string tmp;
      int i = 0;
      while (std::getline(sstm, tmp, ',')) c[i++] = std::stoi(tmp);
    }

    void addLine(const Coord c1, const Coord c2) {
      checkExtents(c1);
      checkExtents(c2);

      int delta, sign;
      Coord step = {0, 0};
      for (int axis = 0; axis < 2; axis++) {
        delta = c2[axis] - c1[axis];
        if (delta) {
          sign = delta/abs(delta);
          step[axis] = sign;
          for (int i = 0; i <= (delta*sign); i++) set(c1[0] + i*step[0], c1[1] + i*step[1], 1);
          break;
        }
      }
    }
};
