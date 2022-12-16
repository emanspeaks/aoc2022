#pragma once

#include <deque>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
// #include <list>

#define FREQSCALAR 4000000LL

class Row {
  public:
    Row(): m_mx(0), m_px(-1) {}

    int cols() {return m_px - m_mx + 1;}

    void checkExtent(int x) {
      if (m_mx > m_px) {
        m_data.push_back(false);
        m_mx = m_px = x;
      } else if (x > m_px) {
        for (int i = x - m_px; i > 0; i--) m_data.push_back(false);
        m_px = x;
      } else if (x < m_mx) {
        for (int i = m_mx - x; i > 0; i--) {
          m_data.push_front(false);
        }
        m_mx = x;
      }
    }

    bool getX(int x) {return m_data[x - m_mx];}
    void setX(int x, bool b) {
      checkExtent(x);
      m_data[x - m_mx] = b;
    }

    void setAllFalse() {
      int len = cols();
      for (int i = 0; i < len; i++) m_data[i] = false;
    }

    int countTrue() {
      int sum = 0;
      for (auto i: m_data) sum += i;
      return sum;
    }

    std::vector<int> listFalseX() {
      std::vector<int> out;
      int len = cols();
      for (int i = 0; i < len; i++) if (!m_data[i]) out.push_back(m_mx + i);
      return out;
    }

  private:
    std::deque<bool> m_data;
    int m_mx, m_px;
};

class SensorMap {
  public:
    SensorMap(): m_n(0) {}

    void parseLine(const std::string &s) {
      std::stringstream sstm(s);
      std::vector<std::string> stok;
      std::string tmp;
      while (std::getline(sstm, tmp, ' ')) stok.push_back(tmp);
      int sx = std::stoi(stok[2].substr(2, stok[2].length() - 3));
      int sy = std::stoi(stok[3].substr(2, stok[3].length() - 3));
      int bx = std::stoi(stok[8].substr(2, stok[8].length() - 3));
      int by = std::stoi(stok[9].substr(2, stok[9].length() - 2));
      addSensor(sx, sy, bx, by);
    }

    void checkY(int y, Row &r) {
      int dy, x;
      for (int i = 0; i < m_n; i++) {
        dy = abs(m_sy[i] - y);

        //check each sensor in turn to see if it may overlap and by how much
        if (dy <= m_rad[i]) {
          for (int dx = 0; dx < (1 + m_rad[i] - dy); dx++) {
            x = m_sx[i] + dx;
            r.setX(x, !isKnownBeacon(x, y));

            x = m_sx[i] - dx;
            r.setX(x, !isKnownBeacon(x, y));
          }
        }
      }
    }

    int unknownBeaconSearch(int min, int max) {
      Row r;
      r.checkExtent(min);
      r.checkExtent(max);
      int y, midy = (max - min) / 2, halfrange = max - midy;
      for (int dy = 0; dy <= halfrange; dy++) {
        for (int sgn = -1; sgn < 2; sgn += 2) {
          y = midy + dy*sgn;
          std::cout << "test: " << y << "\n";
          r.setAllFalse();
          checkY(y, r);
          for (int x = min; x < max; x++) {
            if (!r.getX(x) && !isKnownBeacon(x, y))
              return x*FREQSCALAR + y;
          }
        }
      }
      return -1;
    }

    long long fasterBeaconSearch(int min, int max) {
      std::vector<int> r, sidx;

      //sort sensors geometrically for speed
      for (int i = 0; i < m_n; i++) sidx.push_back(i);
      // std::sort(sidx.begin(), sidx.end(), bySYX);

      int dy, halfwidth, mx, px, j, k, len;
      long long x;
      for (long long y = max; y >= min; y--) {
        if (!(y%10000))
        std::cout << "test: " << y << " (" << 100.0*(y-min)/(max-min) << "%)\n";
        r.clear();
        r.push_back(min - 2);
        r.push_back(max + 2);
        for (auto i: sidx) {
          // compute x range for sensor in row y
          dy = abs(m_sy[i] - y);

          //check each sensor in turn to see if it may overlap and by how much
          halfwidth = m_rad[i] - dy;
          if (halfwidth >= 0) {
            // x values outside cutout
            mx = m_sx[i] - halfwidth - 1;
            px = m_sx[i] + halfwidth + 1;

            // limit to search range
            if (mx < min) mx = min - 1;
            else if (mx > max) break;
            if (px > max) px = max + 1;
            else if (px < min) break;

            j = 0;
            // 1 5 10 20 25 30
            while (mx > r[j]) j++; // j is last element >= mx
            if (!(j % 2) && mx == r[j]) j++;
            if (j % 2) { // odd
              if (px > r[j]) {
                // overlaps with gap, move left side to mx
                r[j] = mx;

                // what about right side?
                k = j + 1;
                while (px > r[k]) k++; // k is last element >= px
                if (k % 2) { // odd
                  // absorbs other gaps completely
                  r[j + 1] = px;
                  if (k - j - 2) {
                    // remove old gaps if we absorb any by getting here
                    for (int q = k - 1; q > j + 1; q--) r.erase(r.begin()+q);
                  }
                } else {
                  // union with/stretch existing gaps
                  if (k - j - 1) {
                    // remove old gaps if we absorb any by getting here
                    for (int q = k - 1; q > j; q--) r.erase(r.begin()+q);
                  }
                }
              } else {
                // new gap before next, insert
                r.insert(r.begin()+j, px);
                r.insert(r.begin()+j, mx);
              }
            } else {
              // we're already in a gap, so just move the right side
              k = j;
              while (px > r[k]) k++; // k is last element >= px
              if (k > j) {
                if (k % 2) { // odd
                  // union with/stretch existing gaps
                  r[j] = px;
                  if (k - j - 1) {
                    // remove old gaps if we absorb any by getting here
                    for (int q = k - 1; q > j; q--) r.erase(r.begin()+q);
                  }
                } else {
                  // absorbs other gaps completely
                  // if (k - j - 2) {
                    // remove old gaps if we absorb any by getting here
                    for (int q = k - 1; q > j - 1; q--) r.erase(r.begin()+q);
                  // }
                }
              }
            }
          }
          if (r.size() == 4 && r[1] < min && r[2] > max) break;
        }
        // see what's left in the row
        len = r.size();
        if (len > 4 || r[1] >= min || (len == 4 && r[2] <= max)) {
          // we got a match, get the x coord
          x = r[2]; // maybe?
          std::cout << "x:" << x << ", y:" << y << ", r[3]:" << r[3] << "\n";
          long long tmp = x*FREQSCALAR + y;
          return tmp;
        }
      }
      return -1;
    }

  private:
    std::vector<int> m_sx, m_sy, m_bx, m_by, m_rad;
    int m_n;

    // bool bySYX(int i, int j) {
    //   // true if i before j
    //   return (m_sy[i] == m_sy[j])? (m_sx[i] < m_sx[j]) : m_sy[i] < m_sy[j];
    // }

    bool isKnownBeacon(int x, int y) {
      for (int i = 0; i < m_n; i++) if (m_by[i] == y && m_bx[i] == x) return true;
      return false;
    }

    int manhattan(int x1, int y1, int x2, int y2) {
      return abs(x2 - x1) + abs(y2 - y1);
    }

    void addSensor(int sx, int sy, int bx, int by) {
      int radius = manhattan(sx, sy, bx, by);
      m_sx.push_back(sx);
      m_sy.push_back(sy);
      m_bx.push_back(bx);
      m_by.push_back(by);
      m_rad.push_back(radius);
      m_n++;
    }
};
