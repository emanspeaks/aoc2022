#pragma once

#include <vector>
#include <string>
#include <deque>
#include <memory>
#include <array>
#include <iostream>
#include <algorithm>

typedef std::array<int, 2> Coord;

template <class T> using YData = std::deque<T>;
template <class T> using XPtr = YData<T>*;
template <class T> using XData = std::deque<std::unique_ptr<YData<T>>>;

template <class T> class ElfMap {
  public:
    ElfMap(): ElfMap(0) {}
    ElfMap(T defaultval): m_mx(0), m_my(0), m_px(-1), m_py(-1), m_defaultval(defaultval) {}

    void printGrid() {
      T v;
      char c;
      for (int y = m_my; y <= m_py; y++) {
        for (int x = m_mx; x <= m_px; x++) {
          v = get(x, y);
          // std::cout << std::to_string(!!v);
          c = (v)? '#' : '.';
          std::cout << c;
        }
        std::cout << "\n";
      }
    }

    void cleanMap() {
      for (int y = m_my; y <= m_py; y++) for (int x = m_mx; x <= m_px; x++)
        if (get(x, y) < 0) set(x, y, 0);
    }

    void getActualExtents(int &mx, int &px, int &my, int &py) {
      mx = m_px;
      px = m_mx;
      my = m_py;
      py = m_my;

      for (int y = m_my; y <= m_py; y++) for (int x = m_mx; x <= m_px; x++) {
        if (get(x, y)) {
          mx = std::min(mx, x);
          px = std::max(px, x);
          my = std::min(my, y);
          py = std::max(py, y);
        }
      }
    }

    T safeGet(int x, int y, T defaultval) {
      if (x > m_px || x < m_mx || y > m_py || y < m_my) return defaultval;
      return get(x, y);
    }

    T safeGet(Coord xy, T defaultval) {
      if (xy[0] > m_px || xy[0] < m_mx || xy[1] > m_py || xy[1] < m_my) return defaultval;
      return get(xy);
    }

    T safeGet(int x, int y) {return safeGet(x, y, m_defaultval);}
    T safeGet(Coord xy) {return safeGet(xy, m_defaultval);}

    void safeSet(int x, int y, T v) {
      checkExtents({x, y});
      set(x, y, v);
    }

    void safeSet(Coord xy, T v) {
      checkExtents(xy);
      set(xy, v);
    }

  private:
    XData<T> m_grid;
    int m_mx, m_my, m_px, m_py;
    T m_defaultval;

    void addPX() {m_grid.push_back(std::make_unique<YData<T>>());}
    void addMX() {m_grid.push_front(std::make_unique<YData<T>>());}

    void addPY() {for (auto &x: m_grid) addPY(x.get());}
    void addPY(XPtr<T> x) {
      x->push_back(m_defaultval);
    }

    void addMY() {for (auto &x: m_grid) addMY(x.get());}
    void addMY(XPtr<T> x) {
      x->push_front(m_defaultval);
    }

    void addAndFillPX() {
      addPX();
      XPtr<T> pxp = m_grid.back().get();
      for (int i = m_py - m_my + 1; i > 0; i--) addPY(pxp);
    }

    void addAndFillMX() {
      addMX();
      XPtr<T> mxp = m_grid.front().get();
      for (int i = m_py - m_my + 1; i > 0; i--) addPY(mxp);
    }

    void set(const Coord c, T v) {
      set(c[0], c[1], v);
    }

    void set(int x, int y, T v) {
      m_grid[x - m_mx].get()->at(y - m_my) = v;
    }

    T get(const Coord c) {
      return get(c[0], c[1]);
    }

    T get(int x, int y) {
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
        int tmp = m_mx;
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
};
